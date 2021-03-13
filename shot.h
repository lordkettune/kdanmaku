// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// *:･ﾟ✧ shot.hpp *:･ﾟ✧
// 
// Shot object. This doesn't really do much on its own; it's mostly just a data object.
// Managed by Danmaku and Pattern nodes.
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

#ifndef SHOT_H
#define SHOT_H

#include "core/object/object.h"

class Pattern;
class Danmaku;

class Shot : public Object {
    GDCLASS(Shot, Object);

public:
    enum {
        FLAG_ACTIVE    = 1,
        FLAG_GRAZING   = 2,
        FLAG_COLLIDING = 4
    };

    unsigned int flags;
    unsigned int effects;

    Pattern* owner;
    int global_id;
    int local_id;

    int time;
    int sprite_id;
    Vector2 global_position;
    Vector2 position;
    Vector2 direction;
    float speed;
    float radius;

    inline void flag(int p_flag)     { flags |= p_flag;  }
    inline void unflag(int p_flag)   { flags &= ~p_flag; }
    inline bool flagged(int p_flag)  { return flags & p_flag; }
    inline bool has_effect(int p_id) { return effects & (1 << p_id); }

    Pattern* get_pattern();
    Danmaku* get_danmaku();

    int get_time();

    void set_effects(Array p_effects);

    void set_sprite(String p_key);
    String get_sprite();

    void set_velocity(Vector2 p_velocity);
    Vector2 get_velocity();

    void set_rotation(float p_rotation);
    float get_rotation();

    static void _register_methods();
    void _init();
};

#endif