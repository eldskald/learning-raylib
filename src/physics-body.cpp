#include "physics-body.h"
#include "game-element.h"
#include <cmath>
#include <list>
#include <raylib.h>
#include <raymath.h>

using namespace std;

list<physics_body*> physics_body::physics_bodies_;

physics_body::physics_body() {
    this->vel = Vector2Zero();
    this->collision_box = (Rectangle){0};
    this->static_body = false;
    this->one_way = false;
    this->collision_layer = 0b00000000;
    this->collision_mask = 0b00000000;
}

list<physics_body*> physics_body::get_colliders(Rectangle collision_box,
                                                bitset<8> mask) {
    list<physics_body*> colliders;
    for (auto i = physics_body::physics_bodies_.begin();
         i != physics_body::physics_bodies_.end();
         ++i) {
        bitset<8> layer_mask_check = mask;
        layer_mask_check &= (*i)->collision_layer;
        if (CheckCollisionRecs(collision_box, (*i)->get_collision_rect_()) &&
            layer_mask_check != 0b00000000) {
            colliders.push_back(*i);
        }
    }
    return colliders;
}

// This function is to get the position in space of the collision shape of any
// given body. We take the floor of everything because colliding with floats is
// inconsistent, so we make every physics rect only move in integer increments.
Rectangle physics_body::get_collision_rect_() {
    return (Rectangle){floor(this->get_global_pos().x + this->collision_box.x),
                       floor(this->get_global_pos().y + this->collision_box.y),
                       floor(this->collision_box.width),
                       floor(this->collision_box.height)};
}

// Calculate where the body would stop if it tried to move delta_d in the X
// axis. Will be equal to delta_d if there are no collisions, or some smaller
// number in case it collides. It is done by getting the rectangle that would
// be the trail of pixels covered by the body as it moved delta_d and finding
// all bodies colliding with it. Bodies already colliding will be ignored.
float physics_body::compute_h_movement_(float delta_d, bool ignore_children) {
    float delta = delta_d;
    if (delta == 0.0f) return 0.0f;

    // The trail. We only take the floor on the Y axis here because we want to
    // be able to move on a float amount, but if we don't take the floor on the
    // other axis, we might detect collisions with bodies that are touching the
    // bottom of this one if it has some fractionary rest because the other one
    // will lose its fraction and go up a little on the calculation.
    Rectangle target_rec =
        (Rectangle){this->get_global_pos().x + this->collision_box.x,
                    floor(this->get_global_pos().y + this->collision_box.y),
                    abs(delta),
                    floor(this->collision_box.height)};
    if (delta > 0.0f)
        target_rec.x += this->collision_box.width;
    else
        target_rec.x += delta;

    // Iterate through all bodies found on the trail.
    list<physics_body*> colliders =
        physics_body::get_colliders(target_rec, this->collision_mask);
    for (auto i = colliders.begin(); i != colliders.end(); ++i) {

        // Ignore the body if it is among the following:
        if (*i == this) continue;
        if ((*i)->descends_from(this) && ignore_children) continue;
        if (CheckCollisionRecs(this->get_collision_rect_(),
                               (*i)->get_collision_rect_()))
            continue;
        if ((*i)->one_way) continue;

        // Find the final delta to be returned.
        if (delta < 0.0f) {
            float body_left =
                floor(this->get_global_pos().x + this->collision_box.x);
            float coll_right =
                floor((*i)->get_global_pos().x + (*i)->collision_box.x +
                      (*i)->collision_box.width);
            delta = max(coll_right - body_left, delta);
        } else {
            float body_right =
                floor(this->get_global_pos().x + this->collision_box.x +
                      this->collision_box.width);
            float coll_left =
                floor((*i)->get_global_pos().x + (*i)->collision_box.x);
            delta = min(coll_left - body_right, delta);
        }
    }

    return delta;
}

// The same thing as compute_h_movement_ except for vertical movements. This
// time we have to take one way colliders into account, we want them to
// collide only if the body is coming from above and is not colliding already.
float physics_body::compute_v_movement_(float delta_d, bool ignore_children) {
    float delta = delta_d;
    if (delta == 0.0f) return 0.0f;

    // The trail. We only take the floor on the X axis here because we want to
    // be able to move on a float amount, but if we don't take the floor on the
    // other axis, we might detect collisions with bodies that are touching the
    // right side of this one if it has some fractionary rest because the other
    // one will lose its fraction and go left a little on the calculation.
    Rectangle target_rec =
        (Rectangle){floor(this->get_global_pos().x + this->collision_box.x),
                    this->get_global_pos().y + this->collision_box.y,
                    floor(this->collision_box.width),
                    abs(delta)};
    if (delta > 0.0f)
        target_rec.y += this->collision_box.height;
    else
        target_rec.y += delta;

    // Iterate through all bodies found on the trail.
    list<physics_body*> colliders =
        physics_body::get_colliders(target_rec, this->collision_mask);
    for (auto i = colliders.begin(); i != colliders.end(); ++i) {

        // Ignore the body if it is among the following:
        if (*i == this) continue;
        if ((*i)->descends_from(this) && ignore_children) continue;
        if (CheckCollisionRecs(this->get_collision_rect_(),
                               (*i)->get_collision_rect_()))
            continue;
        if ((*i)->one_way && delta < 0.0f) continue;

        // Find the final delta to be returned.
        if (delta < 0.0f) {
            float body_top =
                floor(this->get_global_pos().y + this->collision_box.y);
            float coll_bottom =
                floor((*i)->get_global_pos().y + (*i)->collision_box.y +
                      (*i)->collision_box.height);
            delta = max(coll_bottom - body_top, delta);
        } else {
            float body_bottom =
                floor(this->get_global_pos().y + this->collision_box.y +
                      this->collision_box.height);
            float coll_top =
                floor((*i)->get_global_pos().y + (*i)->collision_box.y);
            delta = min(coll_top - body_bottom, delta);
        }
    }

    return delta;
}

float physics_body::move_and_drag_children_h_(float delta_d) {
    if (delta_d == 0.0f) return 0.0f;
    float delta = this->compute_h_movement_(delta_d, true);

    // Order children by their collision box's global position.
    // The logic is: the children will all move on the same direction, so we
    // should first move the ones in front, and then the second most in front,
    // etc etc. This way we stop bodies that are touching to get separated
    // by a gap equal to the delta.
    list<physics_body*> ordered_children;
    list<game_element*> children = this->get_children();
    for (auto i = children.begin(); i != children.end(); ++i) {
        physics_body* child = dynamic_cast<physics_body*>(*i);
        if (child) {
            if (ordered_children.empty())
                ordered_children.push_back(child);
            else {
                float child_col_x =
                    child->get_global_pos().x + child->collision_box.x;
                float top_col_x = ordered_children.back()->get_global_pos().x +
                                  ordered_children.back()->collision_box.x;
                if (child_col_x >= top_col_x)
                    ordered_children.push_back(child);
                else {
                    if (delta >= 0.0f) {
                        for (auto j = ordered_children.begin();
                             j != ordered_children.end();
                             ++j) {
                            float body_col_x = (*j)->get_global_pos().x +
                                               (*j)->collision_box.x;
                            if (child_col_x >= body_col_x) {
                                ordered_children.emplace(j, child);
                                break;
                            }
                        }
                    } else {
                        for (auto j = ordered_children.begin();
                             j != ordered_children.end();
                             ++j) {
                            float body_col_x = (*j)->get_global_pos().x +
                                               (*j)->collision_box.x;
                            if (child_col_x < body_col_x) {
                                ordered_children.emplace(j, child);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    // Call this function on all this body's children, now that they are
    // ordered.
    for (auto i = ordered_children.begin(); i != ordered_children.end(); ++i) {
        (*i)->move_and_drag_children_h_(delta);
    }

    // After we moved all its children, we calculate the movement again, this
    // time allowing it to collide with its children and then move it.
    delta = this->compute_h_movement_(delta);
    this->pos.x += delta;

    // Remember we already moved all the children, but changing this body's
    // position will also change the children's global position, so we have to
    // correct it.
    for (auto i = ordered_children.begin(); i != ordered_children.end(); ++i) {
        (*i)->pos.x -= delta;
    }

    return delta;
}

float physics_body::move_and_drag_children_v_(float delta_d) {
    if (delta_d == 0.0f) return 0.0f;
    float delta = this->compute_v_movement_(delta_d, true);

    // Order children by their collision box's global position.
    // The logic is: the children will all move on the same direction, so we
    // should first move the ones in front, and then the second most in front,
    // etc etc. This way we stop bodies that are touching to get separated
    // by a gap equal to the delta.
    list<physics_body*> ordered_children;
    list<game_element*> children = this->get_children();
    for (auto i = children.begin(); i != children.end(); ++i) {
        physics_body* child = dynamic_cast<physics_body*>(*i);
        if (child) {
            if (ordered_children.empty())
                ordered_children.push_back(child);
            else {
                float child_col_y =
                    child->get_global_pos().y + child->collision_box.y;
                float top_col_y = ordered_children.back()->get_global_pos().y +
                                  ordered_children.back()->collision_box.y;
                if (child_col_y >= top_col_y)
                    ordered_children.push_back(child);
                else {
                    if (delta >= 0.0f) {
                        for (auto j = ordered_children.begin();
                             j != ordered_children.end();
                             ++j) {
                            float body_col_y = (*j)->get_global_pos().y +
                                               (*j)->collision_box.y;
                            if (child_col_y >= body_col_y) {
                                ordered_children.emplace(j, child);
                                break;
                            }
                        }
                    } else {
                        for (auto j = ordered_children.begin();
                             j != ordered_children.end();
                             ++j) {
                            float body_col_y = (*j)->get_global_pos().y +
                                               (*j)->collision_box.y;
                            if (child_col_y < body_col_y) {
                                ordered_children.emplace(j, child);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    // Call this function on all this body's children, now that they are
    // ordered.
    for (auto i = ordered_children.begin(); i != ordered_children.end(); ++i) {
        (*i)->move_and_drag_children_v_(delta);
    }

    // After we moved all its children, we calculate the movement again, this
    // time allowing it to collide with its children and then move it.
    delta = this->compute_v_movement_(delta);
    this->pos.y += delta;

    // Remember we already moved all the children, but changing this body's
    // position will also change the children's global position, so we have to
    // correct it.
    for (auto i = ordered_children.begin(); i != ordered_children.end(); ++i) {
        (*i)->pos.y -= delta;
    }

    return delta;
}

void physics_body::physics_tick_() {
    if (this->static_body) return;

    float delta_x = this->vel.x * GetFrameTime();
    float delta_y = this->vel.y * GetFrameTime();
    delta_x = this->move_and_drag_children_h_(delta_x);
    delta_y = this->move_and_drag_children_v_(delta_y);
    this->vel.x = delta_x * GetFPS();
    this->vel.y = delta_y * GetFPS();
}

// This is for the game class to call on the root scene once and trigger
// all physics ticks in order of parent first.
void physics_body::trigger_physics_tick_(game_element* element) {
    physics_body* body = dynamic_cast<physics_body*>(element);
    if (body) body->physics_tick_();
    list<game_element*> children = element->get_children();
    for (auto i = children.begin(); i != children.end(); ++i) {
        physics_body* child = dynamic_cast<physics_body*>(*i);
        if (child) physics_body::trigger_physics_tick_(child);
    }
}

void physics_body::enter_() {
    physics_body::physics_bodies_.push_back(this);
}

void physics_body::exit_() {
    physics_body::physics_bodies_.remove(this);
}
