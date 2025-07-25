#include "core/player.h"
#include "core/base-unit.h"
#include "core/death-particles.h"
#include "core/game.h"
#include "core/respawn-particles.h"
#include "core/scene-manager.h"
#include "engine/inputs.h"
#include "engine/physics-body.h"
#include "engine/sfx.h"
#include "engine/sprite.h"
#include "imports.h"
#include <raylib.h>

player::player() {
    this->collision_box =
        (Rectangle){0, 0, SPRITESHEET_CELL_SIZE_X, SPRITESHEET_CELL_SIZE_Y};
    this->collision_layer = COL_LAYER_PLAYER;
    this->collision_mask = COL_MASK_PLAYER;
    this->one_way = true;
    auto* player_sprite = new sprite();
    player_sprite->atlas_coords = PLAYER_ATLAS_COORDS;
    player_sprite->tint = MASK_MAIN_COLOR;
    player_sprite->z_index = PLAYER_Z_INDEX;
    this->add_child(player_sprite);
    this->sprite_ = player_sprite;
    auto* death_emitter = new death_particles();
    death_emitter->tint = MASK_MAIN_COLOR;
    death_emitter->pos.y = -SPRITESHEET_CELL_SIZE_Y / 2;
    this->add_child(death_emitter);
    this->death_particles_emitter_ = death_emitter;
    auto* respawn_emitter = new respawn_particles();
    respawn_emitter->tint = MASK_MAIN_COLOR;
    respawn_emitter->pos.y = -SPRITESHEET_CELL_SIZE_Y / 2;
    this->add_child(respawn_emitter);
    this->respawn_particles_emitter_ = respawn_emitter;
}

void player::kill() {
    if (this->killed_) return;
    this->killed_ = true;
    this->collision_layer = 0b00000000;
    this->collision_mask = 0b00000000;
    this->time_ = 0.0f;
    this->sprite_->mark_for_deletion();
    this->sprite_ = nullptr;
    this->death_particles_emitter_->emit();
    sfx::play(sfx::death);
    game_element::trigger_on_player_death_(game::get_root());
}

Vector2 player::get_dir_input_() {
    return (Vector2){(float)(inputs::is_action_down(inputs::action::right)) -
                         (float)(inputs::is_action_down(inputs::action::left)),
                     (float)inputs::is_action_down(inputs::action::down) -
                         (float)(inputs::is_action_down(inputs::action::up))};
}

void player::move_tick_() {
    this->change_dir(player::get_dir_input_());

    if (inputs::is_action_pressed(inputs::action::jump)) this->press_jump();
    if (inputs::is_action_released(inputs::action::jump)) this->release_jump();

    this->base_unit::tick_();
}

void player::history_tick_() {
    for (auto action : {inputs::action::left,
                        inputs::action::right,
                        inputs::action::down,
                        inputs::action::up,
                        inputs::action::jump}) {
        if (inputs::is_action_pressed(action)) {
            this->history_.push_back((input){action, true, this->time_});
        }
        if (inputs::is_action_released(action)) {
            this->history_.push_back((input){action, false, this->time_});
        }
    }
}

void player::shadow_tick_() {
    if (inputs::is_action_pressed(inputs::action::shadow) &&
        scene_manager::get_shadows_limit()) {
        this->history_.push_back(
            (input){inputs::action::shadow, true, this->time_});
        scene_manager::shadow_pressed(this->history_, this);
        this->history_.clear();
        this->time_ = 0.0f;
        for (auto action : {inputs::action::left,
                            inputs::action::right,
                            inputs::action::down,
                            inputs::action::up,
                            inputs::action::jump}) {
            if (inputs::is_action_down(action)) {
                this->history_.push_back((input){action, true, this->time_});
            }
        }
    }
}

void player::tick_() {
    if (this->killed_ && this->time_ >= PLAYER_RESPAWN_TIME) {
        this->mark_for_deletion();
        scene_manager::respawn_player();
    } else if (!this->killed_) {
        this->move_tick_();
        this->history_tick_();
        this->shadow_tick_();
    }
    this->time_ += GetFrameTime();
}

void player::on_player_respawn_() {
    this->respawn_particles_emitter_->emit();
    sfx::play(sfx::respawn);
}

void player::enter_() {
    physics_body::enter_();
    for (auto action : {inputs::action::left,
                        inputs::action::right,
                        inputs::action::down,
                        inputs::action::up,
                        inputs::action::jump}) {
        if (inputs::is_action_down(action)) {
            this->history_.push_back((input){action, true, this->time_});
        }
    }
}

void player::jumped_() {
    sfx::play(sfx::jump);
}
