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

private:
    Danmaku* _danmaku;

    Shot* _grazing_shot;     // Last shot grazed with
    Shot* _colliding_shot;   // Last shot collided with

public:
    float collision_radius;  // Graze radius
    float graze_radius;      // Collision radius    
    bool invulnerable;       // Collisions won't be reported if this is set to true

    Danmaku* get_danmaku() { return _danmaku; }
    Shot* get_colliding_shot() { return _colliding_shot; }
    Shot* get_grazing_shot() { return _grazing_shot; }

    static void _register_methods();
    void _init();

    void _enter_tree();
    void _exit_tree();

    // These are called by Pattern upon a collision
    void hit(Shot* shot);
    void graze(Shot* shot);
};

};

#endif