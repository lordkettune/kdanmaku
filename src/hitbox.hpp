#ifndef __KD_HITBOX_HPP__
#define __KD_HITBOX_HPP__

// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// *:･ﾟ✧ hitbox.hpp *:･ﾟ✧
// 
// Hitbox for the player. Has both a collision radius and a graze radius.
// This object doesn't really do much itself -- the collisions are handled by Patterns.
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

#include <Godot.hpp>
#include <Node2D.hpp>

#include "danmaku.hpp"
#include "shot.hpp"

namespace godot {

class Hitbox : public Node2D {
    GODOT_CLASS(Hitbox, Node2D)

    float collision_radius;
    float graze_radius;
    bool invulnerable;

public:
    Danmaku* get_danmaku();

    Shot* get_colliding_shot();
    Shot* get_grazing_shot();

    float get_collision_radius();
    float get_graze_radius();

    void hit(Shot* p_shot);
    void graze(Shot* p_shot);

    void _enter_tree();
    void _exit_tree();

    static void _register_methods();
    void _init();

private:
    Danmaku* danmaku;
    Shot* grazing_shot;
    Shot* colliding_shot;
};

};

#endif