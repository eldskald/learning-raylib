#pragma once
#include "defs.h"
#include <array>
#include <raylib.h>
#include <string>
#include <unordered_map>

using namespace std;

using map = array<array<int, TILEMAP_SIZE_Y>, TILEMAP_SIZE_X>;

class tilemap {
public:
    tilemap();

    void set_tile(tileset set, int x, int y, int tile_id);
    unordered_map<tileset, map> get_cells();
    string convert_to_data();
    void load_from_data(string data);
    void clear();
    void tick();

private:
    unordered_map<tileset, map> cells_;
    int player_x_{-1};
    int player_y_{-1};
    int goal_x_{-1};
    int goal_y_{-1};
    int water_x_{-1};
    int water_y_{-1};
    int text_1_x_{-1};
    int text_1_y_{-1};
    int text_2_x_{-1};
    int text_2_y_{-1};
    int text_3_x_{-1};
    int text_3_y_{-1};
    int opening_x_{-1};
    int opening_y_{-1};
    int credits_x_{-1};
    int credits_y_{-1};
    int key_gate_top_x_{-1};
    int key_gate_top_y_{-1};

    void highlight_hovered_cell_(int x, int y);
    void render_cell_(int x, int y);
    void click_tile_(int x, int y);
};
