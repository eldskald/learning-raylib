#pragma once
#include "game-element.h"
#include <bitset>
#include <list>
#include <raylib.h>

using namespace std;

class game;

class physics_body : public game_element {
    friend class game;

public:
    Vector2 vel;
    Rectangle collision_box;
    bool static_body;
    bool one_way;
    bitset<8> collision_layer;
    bitset<8> collision_mask;

    static list<physics_body*> get_colliders(Rectangle collision_box,
                                             bitset<8> mask);

    physics_body();

protected:
    void physics_tick_();

    void enter_() override;
    void exit_() override;

private:
    Rectangle get_collision_rect_();
    float compute_h_movement_(float delta_d, bool ignore_children = false);
    float compute_v_movement_(float delta_d, bool ignore_children = false);
    float move_and_drag_children_h_(float delta_d);
    float move_and_drag_children_v_(float delta_d);

    static list<physics_body*> physics_bodies_;

    static void trigger_physics_tick_(game_element* element);
};
