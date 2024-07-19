#pragma once
#include <raylib.h>

class renderer {
public:
    static void initialize();
    static void render();

private:
    static RenderTexture2D base_tex_step_1_;
    static RenderTexture2D base_tex_;
    static RenderTexture2D stretched_tex_;

    static void render_base_();
    static void render_stretched_();
};