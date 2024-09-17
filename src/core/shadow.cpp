#include "core/shadow.h"
#include "core/base-unit.h"
#include "core/death-particles.h"
#include "core/respawn-particles.h"
#include "core/scene-manager.h"
#include "engine/sprite.h"
#include <algorithm>
#include <raylib.h>
#include <utility>

using namespace std;

shadow::shadow(input_history history) : history_(std::move(history)) {
    this->collision_box =
        (Rectangle){0, 0, SPRITESHEET_CELL_SIZE_X, SPRITESHEET_CELL_SIZE_Y};
    this->collision_mask = COL_MASK_PLAYER;
    this->collision_layer = COL_LAYER_PLAYER;
    auto* shadow_sprite = new sprite();
    shadow_sprite->atlas_coords = PLAYER_ATLAS_COORDS;
    shadow_sprite->tint = SHADOW_MASK_COLOR;
    this->add_child(shadow_sprite);
    this->sprite_ = shadow_sprite;
    auto* death_emitter = new death_particles();
    death_emitter->tint = SHADOW_MASK_COLOR;
    death_emitter->pos.y = -SPRITESHEET_CELL_SIZE_Y / 2;
    this->add_child(death_emitter);
    this->death_particles_emitter_ = death_emitter;
    auto* spawn_emitter = new respawn_particles();
    spawn_emitter->tint = SHADOW_MASK_COLOR;
    spawn_emitter->pos.y = -SPRITESHEET_CELL_SIZE_Y / 2;
    this->add_child(spawn_emitter);
    this->spawn_particles_emitter_ = spawn_emitter;
}

void shadow::set_iterator_to_self(list<shadow*>::iterator it) {
    this->iterator_to_self_ = it;
}

list<shadow*>::iterator shadow::get_iterator_to_self() {
    return this->iterator_to_self_;
}

void shadow::emit_spawn_particles() {
    this->spawn_particles_emitter_->emit();
}

void shadow::kill() {
    if (this->killed_) return;
    this->killed_ = true;
    this->death_particles_emitter_->emit();
    this->collision_layer = 0b00000000;
    this->collision_mask = 0b00000000;
    scene_manager::remove_shadow(this);
    this->mark_for_deletion();
}

void shadow::remove() {
    scene_manager::remove_shadow(this);
    this->mark_for_deletion();
}

void shadow::tick_() {
    if (this->history_.empty()) return;

    this->time_ += GetFrameTime();
    input curr = this->history_.front();
    if (this->time_ >= curr.time) {
        this->read_input_(curr);
        this->history_.pop_front();
    }

    this->base_unit::tick_();
}

void shadow::read_input_(input input) {
    switch (input.action) {
    case inputs::action::jump: {
        if (input.pressed)
            this->press_jump();
        else
            this->release_jump();
        break;
    }
    case inputs::action::left: {
        if (input.pressed)
            this->curr_dir_.x -= 1.0f;
        else
            this->curr_dir_.x += 1.0f;
        break;
    }
    case inputs::action::right: {
        if (input.pressed)
            this->curr_dir_.x += 1.0f;
        else
            this->curr_dir_.x -= 1.0f;
        break;
    }
    case inputs::action::up: {
        if (input.pressed)
            this->curr_dir_.y -= 1.0f;
        else
            this->curr_dir_.y += 1.0f;
        break;
    }
    case inputs::action::down: {
        if (input.pressed)
            this->curr_dir_.y += 1.0f;
        else
            this->curr_dir_.y -= 1.0f;
        break;
    }
    default:
        break;
    }
    this->curr_dir_.x = clamp(this->curr_dir_.x, -1.0f, 1.0f);
    this->curr_dir_.y = clamp(this->curr_dir_.y, -1.0f, 1.0f);
    this->change_dir(this->curr_dir_);
}
