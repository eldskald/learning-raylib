#include "player.h"
#include "base-unit.h"
#include "inputs.h"
#include "sprite.h"
#include <raylib.h>

player::player() {
    const float player_width = 8.0f;
    const float player_height = 8.0f;
    const Vector2 player_sprite_coords = (Vector2){1, 5};
    this->collision_box = (Rectangle){0, 0, player_width, player_height};
    this->collision_mask = COL_MASK_PLAYER;
    this->collision_layer = COL_LAYER_PLAYER;
    auto player_sprite = new sprite();
    player_sprite->atlas_coords = player_sprite_coords;
    player_sprite->tint = MAIN_MASK_COLOR;
    this->add_child(player_sprite);
}

void player::tick_() {
    this->time_ += GetFrameTime();

    this->change_dir(inputs::get_dir_input());

    if (inputs::is_action_pressed(inputs::action::jump)) this->press_jump();
    if (inputs::is_action_released(inputs::action::jump)) this->release_jump();

    this->base_unit::tick_();
}
