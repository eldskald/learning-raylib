#include "sprite.h"
#include <raylib.h>

Texture2D sprite::atlas_ = (Texture2D){0};
Shader* sprite::base_shader = new Shader();

sprite::sprite() {
    this->atlas_coords = (Vector2){0, 0};
    this->shader = sprite::base_shader;
}

void sprite::tick_() {
    BeginShaderMode(*(this->shader));
    DrawTexturePro(sprite::atlas_,
                   (Rectangle){SPRITESHEET_CELL_SIZE_X * this->atlas_coords.x,
                               SPRITESHEET_CELL_SIZE_Y * this->atlas_coords.y,
                               SPRITESHEET_CELL_SIZE_X,
                               SPRITESHEET_CELL_SIZE_Y},
                   (Rectangle){this->global_pos_.x,
                               this->global_pos_.y,
                               SPRITESHEET_CELL_SIZE_X * PIXEL_SIZE_X,
                               SPRITESHEET_CELL_SIZE_Y * PIXEL_SIZE_Y},
                   (Vector2){0, 0},
                   0.0f,
                   WHITE);
    EndShaderMode();
}
