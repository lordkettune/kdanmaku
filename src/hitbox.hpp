#ifndef __KD_HITBOX_HPP__
#define __KD_HITBOX_HPP__

#include <Godot.hpp>
#include <Node2D.hpp>

#include "danmaku.hpp"
#include "shot.hpp"

namespace godot {

class Hitbox : public Node2D {
    GODOT_CLASS(Hitbox, Node2D)

private:
    Danmaku* _danmaku;

    Shot* _grazing_shot;
    Shot* _colliding_shot;

public:
    float collision_radius;
    float graze_radius;

    bool invulnerable;

    Danmaku* get_danmaku() { return _danmaku; }
    Shot* get_colliding_shot() { return _colliding_shot; }
    Shot* get_grazing_shot() { return _grazing_shot; }

    static void _register_methods();
    void _init();

    void _enter_tree();
    void _exit_tree();

    void hit(Shot* shot);
    void graze(Shot* shot);
};

};

#endif