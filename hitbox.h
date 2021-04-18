// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// *:･ﾟ✧ hitbox.hpp *:･ﾟ✧
// 
// Hitbox for the player. Has both a collision radius and a graze radius.
// This object doesn't really do much itself -- the collisions are handled by Patterns.
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

#ifndef HITBOX_H
#define HITBOX_H

#include "scene/2d/node_2d.h"

#include "danmaku.h"
#include "shot.h"

class Hitbox : public Node2D {
    GDCLASS(Hitbox, Node2D);

    float collision_radius;
    float graze_radius;
    bool invulnerable;

    Danmaku* danmaku;
    Shot* grazing_shot;
    Shot* colliding_shot;

protected:
    static void _bind_methods();
    void _notification(int p_what);

public:
    void hit(Shot* p_shot);
    void graze(Shot* p_shot);

    Danmaku* get_danmaku() const;
    void remove_from_danmaku();

    void set_invulnerable(bool p_invulnerable);
    bool is_invulnerable() const;

    Shot* get_colliding_shot() const;
    Shot* get_grazing_shot() const;

    void set_collision_radius(float p_collision_radius);
    float get_collision_radius() const;

    void set_graze_radius(float p_graze_radius);
    float get_graze_radius() const;

    Hitbox();
};

#endif