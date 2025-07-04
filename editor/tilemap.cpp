#include "tilemap.h"
#include "defs.h"
#include "editor.h"
#include "spritesheet.h"
#include "tileset-manager.h"
#include <array>
#include <raylib.h>
#include <string>
#include <unordered_map>

using namespace std;

const int CONVERT_TO_DATA_CONST = 50;
const int PLAYER_TILE_ID = 0;
const int GOAL_TILE_ID = 1;
const int OPENING_TILE_ID = 2;
const int CREDITS_TILE_ID = 3;
const int WATER_TILE_ID = 4;
const int TEXT_1_TILE_ID = 5;
const int TEXT_2_TILE_ID = 6;
const int TEXT_3_TILE_ID = 7;
const int KEY_GATE_TOP_ID = 8;
const int FIRST_SHADOW_ID = 24;

tilemap::tilemap() {
    for (auto set : TILESETS) {
        map map;
        for (int i = 0; i < TILEMAP_SIZE_X; i++) {
            for (int j = 0; j < TILEMAP_SIZE_Y; j++) {
                map.at(i).at(j) = -1;
            }
        }
        this->cells_.insert({set, map});
    }
}

void tilemap::set_tile(tileset set, int x, int y, int tile_id) {
    // There can only be one player type tile on the map
    if (set == tileset::interact && tile_id == -1 &&
        this->cells_.at(set).at(x).at(y) == PLAYER_TILE_ID) {
        this->player_x_ = -1;
        this->player_y_ = -1;
    }
    if (set == tileset::interact && tile_id == PLAYER_TILE_ID) {
        if (this->player_x_ != -1 && this->player_y_ != -1) {
            this->cells_.at(set).at(this->player_x_).at(this->player_y_) = -1;
        }
        this->player_x_ = x;
        this->player_y_ = y;
    }

    // There can only be one goal type tile on the map
    if (set == tileset::interact && tile_id == -1 &&
        this->cells_.at(set).at(x).at(y) == GOAL_TILE_ID) {
        this->goal_x_ = -1;
        this->goal_y_ = -1;
    }
    if (set == tileset::interact && tile_id == GOAL_TILE_ID) {
        if (this->goal_x_ != -1 && this->goal_y_ != -1) {
            this->cells_.at(set).at(this->goal_x_).at(this->goal_y_) = -1;
        }
        this->goal_x_ = x;
        this->goal_y_ = y;
    }

    // There can only be one water type tile on the map
    if (set == tileset::interact && tile_id == -1 &&
        this->cells_.at(set).at(x).at(y) == WATER_TILE_ID) {
        this->water_x_ = -1;
        this->water_y_ = -1;
    }
    if (set == tileset::interact && tile_id == WATER_TILE_ID) {
        if (this->water_x_ != -1 && this->water_y_ != -1) {
            this->cells_.at(set).at(this->water_x_).at(this->water_y_) = -1;
        }
        this->water_x_ = x;
        this->water_y_ = y;
    }

    // There can only be one text1 type tile on the map
    if (set == tileset::interact && tile_id == -1 &&
        this->cells_.at(set).at(x).at(y) == TEXT_1_TILE_ID) {
        this->text_1_x_ = -1;
        this->text_1_y_ = -1;
    }
    if (set == tileset::interact && tile_id == TEXT_1_TILE_ID) {
        if (this->text_1_x_ != -1 && this->text_1_y_ != -1) {
            this->cells_.at(set).at(this->text_1_x_).at(this->text_1_y_) = -1;
        }
        this->text_1_x_ = x;
        this->text_1_y_ = y;
    }

    // There can only be one text2 type tile on the map
    if (set == tileset::interact && tile_id == -1 &&
        this->cells_.at(set).at(x).at(y) == TEXT_2_TILE_ID) {
        this->text_2_x_ = -1;
        this->text_2_y_ = -1;
    }
    if (set == tileset::interact && tile_id == TEXT_2_TILE_ID) {
        if (this->text_2_x_ != -1 && this->text_2_y_ != -1) {
            this->cells_.at(set).at(this->text_2_x_).at(this->text_2_y_) = -1;
        }
        this->text_2_x_ = x;
        this->text_2_y_ = y;
    }

    // There can only be one text3 type tile on the map
    if (set == tileset::interact && tile_id == -1 &&
        this->cells_.at(set).at(x).at(y) == TEXT_3_TILE_ID) {
        this->text_3_x_ = -1;
        this->text_3_y_ = -1;
    }
    if (set == tileset::interact && tile_id == TEXT_3_TILE_ID) {
        if (this->text_3_x_ != -1 && this->text_3_y_ != -1) {
            this->cells_.at(set).at(this->text_3_x_).at(this->text_3_y_) = -1;
        }
        this->text_3_x_ = x;
        this->text_3_y_ = y;
    }

    // There can only be one opening type tile on the map
    if (set == tileset::interact && tile_id == -1 &&
        this->cells_.at(set).at(x).at(y) == OPENING_TILE_ID) {
        this->opening_x_ = -1;
        this->opening_y_ = -1;
    }
    if (set == tileset::interact && tile_id == OPENING_TILE_ID) {
        if (this->opening_x_ != -1 && this->opening_y_ != -1) {
            this->cells_.at(set).at(this->opening_x_).at(this->opening_y_) = -1;
        }
        this->opening_x_ = x;
        this->opening_y_ = y;
    }

    // There can only be one credits type tile on the map
    if (set == tileset::interact && tile_id == -1 &&
        this->cells_.at(set).at(x).at(y) == CREDITS_TILE_ID) {
        this->opening_x_ = -1;
        this->opening_y_ = -1;
    }
    if (set == tileset::interact && tile_id == CREDITS_TILE_ID) {
        if (this->credits_x_ != -1 && this->credits_y_ != -1) {
            this->cells_.at(set).at(this->credits_x_).at(this->credits_y_) = -1;
        }
        this->credits_x_ = x;
        this->credits_y_ = y;
    }

    // There can only be one key_gate_top type tile on the map
    if (set == tileset::interact && tile_id == -1 &&
        this->cells_.at(set).at(x).at(y) == KEY_GATE_TOP_ID) {
        this->key_gate_top_x_ = -1;
        this->key_gate_top_y_ = -1;
    }
    if (set == tileset::interact && tile_id == KEY_GATE_TOP_ID) {
        if (this->key_gate_top_x_ != -1 && this->key_gate_top_y_ != -1) {
            this->cells_.at(set)
                .at(this->key_gate_top_x_)
                .at(this->key_gate_top_y_) = -1;
        }
        this->key_gate_top_x_ = x;
        this->key_gate_top_y_ = y;
    }

    this->cells_.at(set).at(x).at(y) = tile_id;
}

unordered_map<tileset, map> tilemap::get_cells() {
    unordered_map<tileset, map> res;
    for (tileset set : TILESETS) {
        map clone;
        for (int i = 0; i < TILEMAP_SIZE_X; i++) {
            for (int j = 0; j < TILEMAP_SIZE_Y; j++) {
                clone.at(i).at(j) = this->cells_.at(set).at(i).at(j);
            }
        }
        res.insert({set, clone});
    }
    return res;
}

string tilemap::convert_to_data() {
    string data = "";
    for (tileset set : TILESETS) {
        for (int i = 0; i < TILEMAP_SIZE_X; i++) {
            for (int j = 0; j < TILEMAP_SIZE_Y; j++) {
                data += (char)(this->cells_.at(set).at(i).at(j) +
                               CONVERT_TO_DATA_CONST);
            }
        }
    }
    return data;
}

void tilemap::load_from_data(string data) {
    for (tileset set : TILESETS) {
        for (int i = 0; i < TILEMAP_SIZE_X; i++) {
            for (int j = 0; j < TILEMAP_SIZE_Y; j++) {
                int tile_code =
                    (int)(data.at((int)set * TILEMAP_SIZE_X * TILEMAP_SIZE_Y +
                                  TILEMAP_SIZE_Y * i + j) -
                          CONVERT_TO_DATA_CONST);
                this->cells_.at(set).at(i).at(j) = tile_code;
                if (tile_code == PLAYER_TILE_ID) {
                    this->player_x_ = i;
                    this->player_y_ = j;
                } else if (tile_code == GOAL_TILE_ID) {
                    this->goal_x_ = i;
                    this->goal_y_ = j;
                } else if (tile_code == WATER_TILE_ID) {
                    this->water_x_ = i;
                    this->water_y_ = j;
                } else if (tile_code == OPENING_TILE_ID) {
                    this->opening_x_ = i;
                    this->opening_y_ = j;
                } else if (tile_code == CREDITS_TILE_ID) {
                    this->credits_x_ = i;
                    this->credits_y_ = j;
                } else if (tile_code == TEXT_1_TILE_ID) {
                    this->text_1_x_ = i;
                    this->text_1_y_ = j;
                } else if (tile_code == TEXT_2_TILE_ID) {
                    this->text_2_x_ = i;
                    this->text_2_y_ = j;
                } else if (tile_code == TEXT_3_TILE_ID) {
                    this->text_3_x_ = i;
                    this->text_3_y_ = j;
                }
            }
        }
    }
}

void tilemap::highlight_hovered_cell_(int x, int y) {
    if (x == -1 && y == -1) return;
    DrawRectangle(x * CELL_SIZE_X + TILEMAP_ORIGIN_X,
                  y * CELL_SIZE_Y + TILEMAP_ORIGIN_Y,
                  CELL_SIZE_X,
                  CELL_SIZE_Y,
                  CELL_HIGHLIGHT_COLOR);
}

void tilemap::render_cell_(int x, int y) {
    for (tileset set : TILESETS) {
        int id = this->cells_.at(set).at(x).at(y);
        if (id == -1) continue;
        Vector2 coords = tileset_manager::get_tile_sprite_coords(set, id);
        spritesheet::render_sprite_at(
            coords,
            (Vector2){(float)(x * CELL_SIZE_X + TILEMAP_ORIGIN_X),
                      (float)(y * CELL_SIZE_Y + TILEMAP_ORIGIN_Y)},
            id >= FIRST_SHADOW_ID && set == tileset::interact ? FOCUSED_COLOR
                                                              : FG_COLOR);
    }
}

void tilemap::click_tile_(int x, int y) {
    if (x == -1 && y == -1) return;
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        this->set_tile(
            tileset_manager::selected_set,
            x,
            y,
            tileset_manager::selected_tile.at(tileset_manager::selected_set));
    } else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        this->set_tile(tileset_manager::selected_set, x, y, -1);
    } else if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) {
        tileset_manager::selected_tile.at(tileset_manager::selected_set) =
            this->cells_.at(tileset_manager::selected_set).at(x).at(y);
    }
}

void tilemap::clear() {
    for (int i = 0; i < TILEMAP_SIZE_X; i++) {
        for (int j = 0; j < TILEMAP_SIZE_Y; j++) {
            this->set_tile(tileset::blocks, i, j, -1);
            this->set_tile(tileset::background, i, j, -1);
            this->set_tile(tileset::interact, i, j, -1);
        }
    }
}

void tilemap::tick() {

    // Render tiles
    for (int i = 0; i < TILEMAP_SIZE_X; i++) {
        for (int j = 0; j < TILEMAP_SIZE_Y; j++) {
            this->render_cell_(i, j);
        }
    }

    if (editor::mouse_disabled) return;

    // Make mouse equal to its cell coords or (-1, -1) if it's outside
    // the range of the tilemap
    int mouse_x = GetMouseX();
    int mouse_y = GetMouseY();
    if (mouse_x < TILEMAP_ORIGIN_X ||
        mouse_x >= TILEMAP_ORIGIN_X + TILEMAP_PIXEL_SIZE_X ||
        mouse_y < TILEMAP_ORIGIN_Y ||
        mouse_y >= TILEMAP_ORIGIN_Y + TILEMAP_PIXEL_SIZE_Y) {
        mouse_x = -1;
        mouse_y = -1;
    } else {
        mouse_x = (mouse_x - TILEMAP_ORIGIN_X) / CELL_SIZE_X;
        mouse_y = (mouse_y - TILEMAP_ORIGIN_Y) / CELL_SIZE_Y;
    }

    this->highlight_hovered_cell_(mouse_x, mouse_y);
    this->click_tile_(mouse_x, mouse_y);
}
