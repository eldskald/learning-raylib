#pragma once
#include <list>
#include <raylib.h>

using namespace std;

class game;

class game_element {
    friend class game;

public:
    Vector2 pos;

    game_element();
    ~game_element();

    game_element* get_parent();
    list<game_element*> get_children();
    void add_child(game_element* element);
    void remove_child(game_element* child);

protected:
    Vector2 global_pos_;

    void update_pos_();

private:
    game_element* parent_;
    list<game_element*> children_;

    virtual void tick_();
};
