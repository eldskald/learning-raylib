#include "button.h"
#include "defs.h"
#include <raylib.h>
#include <string>

void button::empty_on_click_() {
}

bool button::is_being_hovered_() {
    bool x_check = (GetMouseX() >= (int)this->rect.x) &&
                   (GetMouseX() <= (int)this->rect.x + (int)this->rect.width);
    bool y_check = (GetMouseY() >= (int)this->rect.y) &&
                   (GetMouseY() <= (int)this->rect.y + (int)this->rect.height);
    return x_check && y_check;
}

void button::render_() {
    Color col = FG_COLOR;
    if (this->is_being_hovered_()) {
        col = IsMouseButtonDown(MOUSE_BUTTON_LEFT) ? BTN_PRESSED_COLOR
                                                   : BTN_HOVERED_COLOR;
    }
    if (this->is_being_hovered_() && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        DrawRectangleLinesEx((Rectangle){this->rect.x + 2,
                                         this->rect.y + 2,
                                         this->rect.width - 4,
                                         this->rect.height - 4},
                             1,
                             col);
        Vector2 text_size = MeasureTextEx(
            GetFontDefault(), this->label.c_str(), FONT_SIZE - 2, 0);
        DrawText(this->label.c_str(),
                 (int)this->rect.x + (int)this->rect.width / 2 -
                     (int)text_size.x / 2,
                 (int)this->rect.y + (int)this->rect.height / 2 -
                     (int)text_size.y / 2,
                 FONT_SIZE - 2,
                 col);
    } else {
        DrawRectangleLinesEx(this->rect, 1, col);
        Vector2 text_size =
            MeasureTextEx(GetFontDefault(), this->label.c_str(), FONT_SIZE, 0);
        DrawText(this->label.c_str(),
                 (int)this->rect.x + (int)this->rect.width / 2 -
                     (int)text_size.x / 2,
                 (int)this->rect.y + (int)this->rect.height / 2 -
                     (int)text_size.y / 2,
                 FONT_SIZE,
                 col);
    }

    // Set cursor when being hovered
    if (this->is_being_hovered_()) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    };
}

void button::detect_clicks_() {
    if (this->is_being_hovered_() && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        this->being_clicked_ = true;
    } else if (this->being_clicked_ && IsMouseButtonUp(MOUSE_BUTTON_LEFT)) {
        this->being_clicked_ = false;
        this->on_click();
    } else {
        this->being_clicked_ = false;
    }
}

void button::tick() {
    this->render_();
    this->detect_clicks_();
}
