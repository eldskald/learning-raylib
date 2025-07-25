#pragma once
#include "tilemap.h"
#include <raylib.h>
#include <string>
#include <unordered_map>

using namespace std;

class data_exporter {
public:
    static string get_export_text(unordered_map<tileset, map> cells,
                                  string level_text_1,
                                  string text_1_hidden_for,
                                  string level_text_2,
                                  string text_2_hidden_for,
                                  string level_text_3,
                                  string text_3_hidden_for,
                                  string level_shadows,
                                  string screen_colors_shader,
                                  string level_bgm);

private:
    static int current_id_count_;

    static string get_out_of_bounds_walls_();
    static Vector2* find_next_block_start_(map* cells);
    static string get_block_from_map_(Vector2 start, map* cells);
    static string get_physics_bodies_text_(map* cells);
    static string get_blocks_sprites_text_(map* cells);
    static Vector2* find_next_platform_start_(map* cells);
    static string get_platform_from_map_(Vector2 start, map* cells);
    static string get_platform_bodies_text_(map* cells);
    static string get_platform_sprites_text_(map* cells);
    static Vector2* find_next_shadow_platform_start_(map* cells);
    static string get_shadow_platform_from_map_(Vector2 start, map* cells);
    static string get_shadow_platform_bodies_text_(map* cells);
    static string get_shadow_platform_sprites_text_(map* cells);
    static string get_bg_props_text_(map* cells);
    static string get_bg_grass_text_(map* cells);
    static string get_bg_stars_text_(map* cells);
    static string get_bg_leaves_text_(map* cells);
    static string get_bg_drips_text_(map* cells);
    static string get_bg_puffs_text_(map* cells);
    static string get_bg_pollens_text_(map* cells);
    static string get_bg_raindrops_text_(map* cells);
    static string get_bg_wisps_text_(map* cells);
    static string get_player_text_(map* cells);
    static string get_goal_text_(map* cells);
    static string get_spikes_text_(map* cells);
    static string get_shadow_spikes_text_(map* cells);
    static string get_water_text_(map* cells);
    static string get_key_gate_text_(map* cells);
    static string
    get_level_text_1_text_(map* cells, string level_text, string hidden_for);
    static string
    get_level_text_2_text_(map* cells, string level_text, string hidden_for);
    static string
    get_level_text_3_text_(map* cells, string level_text, string hidden_for);
    static string get_opening_obj_text_(map* cells);
    static string get_credits_obj_text_(map* cells);
    static string get_level_shadow_totals_text_(string level_shadows);
    static string get_screen_colors_shader_text_(string screen_colors_shader);
    static string get_bgm_text_(string level_bgm);
};
