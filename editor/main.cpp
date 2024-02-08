#include "defs.h"
#include "editor.h"
#include <raylib.h>

int main() {
    InitWindow(EDITOR_WINDOW_SIZE_X, EDITOR_WINDOW_SIZE_Y, EDITOR_WINDOW_TITLE);
    SetTargetFPS(EDITOR_FPS);

    editor::initialize();
    while (!WindowShouldClose()) {
        BeginDrawing();
        editor::tick();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
