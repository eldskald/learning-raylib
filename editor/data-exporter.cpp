#include "data-exporter.h"
#include "defs.h"
#include "tilemap.h"
#include "tileset_manager.h"
#include <array>
#include <raylib.h>
#include <string>
#include <unordered_map>

Vector2* data_exporter::find_next_block_start_(map* cells) {
    for (int i = 0; i < TILEMAP_SIZE_X; i++) {
        for (int j = 0; j < TILEMAP_SIZE_Y; j++) {
            if (cells->at(i).at(j) == -1) {
                continue;
            } else {
                auto start = new Vector2();
                start->x = (float)i;
                start->y = (float)j;
                return start;
            }
        }
    }
    return nullptr;
}

string data_exporter::get_block_from_map_(Vector2 start, map* cells) {

    // Initiate block position and shape
    Rectangle rect = (Rectangle){start.x, start.y, 1.0f, 0.0f};

    // Let's find the block height
    int coords_x = (int)start.x;
    int coords_y = (int)start.y;
    while (true) {
        if (cells->at(coords_x).at(coords_y) == -1) {
            break;
        } else {
            cells->at(coords_x).at(coords_y) = -1;
            rect.height += 1.0f;
            coords_y++;
            if (coords_y == TILEMAP_SIZE_Y) break;
        }
    }

    // Now we check if the whole column at the right of the one we just removed
    // is also composed of solid blocks. If so, we remove them from the map and
    // add 1 to rect width. Then we do it again until we find one that's not
    // entirely made of solid blocks.
    coords_y = (int)start.y;
    while (true) {
        coords_x++;
        if (coords_x >= TILEMAP_SIZE_X) break;

        // Checks if the next column is solid
        bool is_column_solid_blocks = true;
        for (int i = 0; i < (int)rect.height; i++) {
            if (cells->at(coords_x).at(coords_y + i) == -1) {
                is_column_solid_blocks = false;
                break;
            }
        }

        // Breaks if it isn't
        if (!is_column_solid_blocks) break;

        // If it is, increase block width and remove the column from the map
        rect.width += 1.0;
        for (int i = 0; i < (int)rect.height; i++) {
            cells->at(coords_x).at(coords_y + i) = -1;
        }
    }

    // We've been treating the rect in cell unit lengths. We should transform
    // them to pixel before exporting data.
    rect.x *= SPRITESHEET_CELL_X;
    rect.y *= SPRITESHEET_CELL_Y;
    rect.width *= SPRITESHEET_CELL_X;
    rect.height *= SPRITESHEET_CELL_Y;

    // Make the return string based on the calculated rect
    string str = "[physics_body]\n";
    str += "pos = (" + to_string((int)rect.x) + "," + to_string((int)rect.y) +
           ")\n";
    str += "type = fixed\n";
    str += "collision_box = (0,0," + to_string((int)rect.width) + "," +
           to_string((int)rect.height) + ")\n";
    str += "collision_layer = 00000100\n";
    return str;
}

string data_exporter::get_physics_bodies_text_(map* cells) {
    string text = "";
    Vector2* start = data_exporter::find_next_block_start_(cells);
    while (start) {
        text += data_exporter::get_block_from_map_(*start, cells);
        text += '\n';
        delete start;
        start = data_exporter::find_next_block_start_(cells);
    }
    return text;
}

string data_exporter::get_blocks_sprites_text_(map* cells) {
    string text = "";
    for (int i = 0; i < TILEMAP_SIZE_X; i++) {
        for (int j = 0; j < TILEMAP_SIZE_Y; j++) {
            if (cells->at(i).at(j) == -1) {
                continue;
            } else {
                Vector2 coords = tileset_manager::get_tile_sprite_coords(
                    tileset::blocks, cells->at(i).at(j));
                text += "[sprite]\n";
                text += "pos = (" + to_string(i * SPRITESHEET_CELL_X) + "," +
                        to_string(j * SPRITESHEET_CELL_Y) + ")\n";
                text += "atlas_coords = (" + to_string((int)coords.x) + "," +
                        to_string((int)coords.y) + ")\n";
                text += "tint = (255,0,0,255)\n";
                text += "\n";
            }
        }
    }
    return text;
}

string data_exporter::get_bg_props_text_(map* cells) {
    string text = "";
    for (int i = 0; i < TILEMAP_SIZE_X; i++) {
        for (int j = 0; j < TILEMAP_SIZE_Y; j++) {
            int cell_id = cells->at(i).at(j);
            if (cell_id == -1) continue;
            tile data =
                tileset_manager::get_tile_data(tileset::background, cell_id);
            if (data.type != tile_type::prop) continue;
            Vector2 coords = data.spritesheet_coords;
            text += "[sprite]\n";
            text += "pos = (" + to_string(i * SPRITESHEET_CELL_X) + "," +
                    to_string(j * SPRITESHEET_CELL_Y) + ")\n";
            text += "atlas_coords = (" + to_string((int)coords.x) + "," +
                    to_string((int)coords.y) + ")\n";
            text += "tint = (0,255,0,255)\n";
            text += "\n";
        }
    }
    return text;
}

string data_exporter::get_bg_grass_text_(map* cells) {
    string text = "";
    for (int i = 0; i < TILEMAP_SIZE_X; i++) {
        for (int j = 0; j < TILEMAP_SIZE_Y; j++) {
            int cell_id = cells->at(i).at(j);
            if (cell_id == -1) continue;
            tile data =
                tileset_manager::get_tile_data(tileset::background, cell_id);
            if (data.type != tile_type::grass) continue;
            Vector2 coords = data.spritesheet_coords;
            string frame_1_str = "(" + to_string((int)coords.x) + "," +
                                 to_string((int)coords.y) + ")";
            string frame_2_str = "(" + to_string((int)coords.x) + "," +
                                 to_string((int)coords.y + 1) + ")";
            text += "[sprite]\n";
            text += "pos = (" + to_string(i * SPRITESHEET_CELL_X) + "," +
                    to_string(j * SPRITESHEET_CELL_Y) + ")\n";
            text += "atlas_coords = " + frame_1_str + "\n";
            text += "tint = (0,255,0,255)\n";
            text += "animation = (" + frame_1_str + ",1.5);(" + frame_2_str +
                    ",1.0);(" + frame_1_str + ",1.0);(" + frame_2_str +
                    ",0.75)\n";
            text += "animation_starting_phase = " +
                    to_string((float)i * GRASS_PHASE_DIFF_PER_TILE) + "\n";
            text += "\n";
        }
    }
    return text;
}

string data_exporter::get_bg_waterfall_text_(map* cells) {
    string text = "";
    for (int i = 0; i < TILEMAP_SIZE_X; i++) {
        for (int j = 0; j < TILEMAP_SIZE_Y; j++) {
            int cell_id = cells->at(i).at(j);
            if (cell_id == -1) continue;
            tile data =
                tileset_manager::get_tile_data(tileset::background, cell_id);
            if (data.type != tile_type::waterfall) continue;
            Vector2 coords = data.spritesheet_coords;
            string frame_1_str = "(" + to_string((int)coords.x) + "," +
                                 to_string((int)coords.y) + ")";
            string frame_2_str = "(" + to_string((int)coords.x) + "," +
                                 to_string((int)coords.y + 1) + ")";
            text += "[sprite]\n";
            text += "pos = (" + to_string(i * SPRITESHEET_CELL_X) + "," +
                    to_string(j * SPRITESHEET_CELL_Y) + ")\n";
            text += "atlas_coords = " + frame_1_str + "\n";
            text += "tint = (0,255,0,255)\n";
            text += "animation = (" + frame_1_str + ",0.15);(" + frame_2_str +
                    ",0.15)\n";
            text += "\n";
        }
    }
    return text;
}

string data_exporter::get_export_text(unordered_map<tileset, map> cells) {
    string data = "";
    data += data_exporter::get_bg_props_text_(&cells.at(tileset::background));
    data += data_exporter::get_bg_grass_text_(&cells.at(tileset::background));
    data += data_exporter::get_bg_waterfall_text_(&cells.at(tileset::background));
    data += data_exporter::get_blocks_sprites_text_(&cells.at(tileset::blocks));
    data += data_exporter::get_physics_bodies_text_(&cells.at(tileset::blocks));
    return data;
}
