#pragma once
#include "core/base-unit.h"
#include "core/death-particles.h"
#include "core/respawn-particles.h"
#include "engine/sprite.h"
#include <raylib.h>
#ifdef DEV
#include <string>
#endif

class player : public base_unit {
public:
#ifdef DEV
    std::string debug_name{"player"};
#endif

    player();

    void kill() override;

protected:
    void tick_() override;
    void enter_() override;
    void on_player_respawn_() override;
    void jumped_() override;

private:
    float time_{0.0f};
    bool killed_{false};
    death_particles* death_particles_emitter_{nullptr};
    respawn_particles* respawn_particles_emitter_{nullptr};
    sprite* sprite_{nullptr};
    input_history history_;

    void move_tick_();
    void history_tick_();
    void shadow_tick_();

    static Vector2 get_dir_input_();
};
