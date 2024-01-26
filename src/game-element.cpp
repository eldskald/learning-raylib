#include "game-element.h"
#include <list>
#include <raymath.h>

using namespace std;

game_element::~game_element() {
    if (this->parent_) this->parent_->remove_child(this);
    for (auto& child : this->children_) {
        delete child;
    }
}

game_element* game_element::get_parent() {
    return this->parent_;
}

list<game_element*> game_element::get_children() {
    list<game_element*> duplicate;
    for (auto& child : this->children_) {
        duplicate.push_back(child);
    }
    return duplicate;
}

void game_element::add_child(game_element* element) {
    if (element->parent_) {
        return;
    }
    element->parent_ = this;
    this->children_.push_back(element);
    if (this->is_on_tree_) game_element::trigger_enter_(element);
}

void game_element::remove_child(game_element* child) {
    if (child->parent_ == this) {
        this->children_.remove(child);
        child->parent_ = nullptr;
        game_element::trigger_exit_(child);
    }
}

bool game_element::descends_from(game_element* element) {
    game_element* curr_parent = this->parent_;
    while (curr_parent != nullptr) {
        if (curr_parent == element) return true;
        curr_parent = curr_parent->parent_;
    }
    return false;
}

bool game_element::is_on_tree() {
    return this->is_on_tree_;
}

Vector2 game_element::get_global_pos() {
    Vector2 global_pos = this->pos;
    game_element* parent = this->get_parent();
    while (parent) {
        global_pos = Vector2Add(global_pos, parent->pos);
        parent = parent->get_parent();
    }
    return global_pos;
}

void game_element::enter_() {
    return;
}

void game_element::exit_() {
    return;
}

void game_element::tick_() {
    return;
}

void game_element::trigger_enter_(game_element* element) {
    element->is_on_tree_ = true;
    element->enter_();
    for (auto& child : element->children_) {
        game_element::trigger_enter_(child);
    }
}

void game_element::trigger_exit_(game_element* element) {
    element->is_on_tree_ = false;
    element->exit_();
    for (auto& child : element->children_) {
        game_element::trigger_exit_(child);
    }
}

void game_element::trigger_tick_(game_element* element) {
    for (auto& child : element->children_) {
        game_element::trigger_tick_(child);
    }
    element->tick_();
}
