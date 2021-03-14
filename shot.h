// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// *:･ﾟ✧ shot.hpp *:･ﾟ✧
// 
// Shot object. This doesn't really do much on its own; it's mostly just a data object.
// Managed by Danmaku and Pattern nodes.
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

#ifndef SHOT_H
#define SHOT_H

#include "core/object.h"

class Pattern;
class Danmaku;

class Shot : public Object {
    GDCLASS(Shot, Object);

    uint32_t flags;
    uint32_t effects;

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

protected:
    static void _bind_methods();

public:
    enum {
        FLAG_ACTIVE    = 1,
        FLAG_GRAZING   = 2,
        FLAG_COLLIDING = 4
    };    

    _FORCE_INLINE_ void flag(int p_flag)     { flags |= p_flag;  }
    _FORCE_INLINE_ void unflag(int p_flag)   { flags &= ~p_flag; }
    _FORCE_INLINE_ bool flagged(int p_flag)  { return flags & p_flag; }
    _FORCE_INLINE_ bool has_effect(int p_id) { return effects & (1 << p_id); }

    Pattern* get_pattern() const;
    Danmaku* get_danmaku() const;

    void set_effects(const Vector<int>& p_effects);
    
    void set_time(int p_time);
    int get_time() const;

    void set_sprite(const StringName& p_key);
    StringName get_sprite() const;

    void set_speed(float p_speed);
    float get_speed() const;

    void set_direction(const Vector2& p_direction);
    Vector2 get_direction() const;

    void set_position(const Vector2& p_position);
    Vector2 get_position() const;

    void set_radius(float p_radius);
    float get_radius() const;

    void set_velocity(const Vector2& p_velocity);
    Vector2 get_velocity() const;

    void set_rotation(float p_rotation);
    float get_rotation() const;
};

#endif