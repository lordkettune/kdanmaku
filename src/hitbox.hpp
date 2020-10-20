#ifndef __KD_HITBOX_HPP__
#define __KD_HITBOX_HPP__

#include <Godot.hpp>
#include <Node2D.hpp>

#include "danmaku.hpp"

namespace godot {

class Hitbox : public Node2D {
    GODOT_CLASS(Hitbox, Node2D)

private:
    Danmaku* _danmaku;

public:
    float radius;
    float graze_radius;
    bool invulnerable;

    Danmaku* get_danmaku() { return _danmaku; }

    static void _register_methods();
    void _init();

    void _enter_tree();
    void _exit_tree();

    void hit();
    void graze();
};

};

#endif