// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// *:･ﾟ✧ shot.hpp *:･ﾟ✧
// 
// Shot object. This doesn't really do much on its own; it's mostly just a data object.
// Managed by Danmaku and Pattern nodes.
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

#ifndef SHOT_H
#define SHOT_H

#define SHOT_REGISTERS 8

#include "shot_effect.h"
#include "core/object.h"

class Pattern;
class Danmaku;

class Shot : public Object {
    GDCLASS(Shot, Object);

    int id;
    Pattern* owner;
    uint32_t flags;

    Variant registers[SHOT_REGISTERS];

    int time;
    int sprite;
    Vector2 global_position;
    Vector2 position;
    Vector2 direction;
    float speed;
    float radius;

protected:
    static void _bind_methods();

public:
    enum {
        SPEED = (2 | (0 << 2))
    };

    enum {
        FLAG_ACTIVE    = 1,
        FLAG_GRAZING   = 2,
        FLAG_COLLIDING = 4
    };

    _FORCE_INLINE_ void flag(int p_flag)     { flags |= p_flag;  }
    _FORCE_INLINE_ void unflag(int p_flag)   { flags &= ~p_flag; }
    _FORCE_INLINE_ bool flagged(int p_flag)  { return flags & p_flag; }

    _FORCE_INLINE_ int get_id() { return id; }
    
    _FORCE_INLINE_ void set_sprite_id(int p_sprite) { sprite = p_sprite; }
    _FORCE_INLINE_ int get_sprite_id() { return sprite; }

    _FORCE_INLINE_ void set_register(Register p_reg, const Variant& p_value) {
        registers[p_reg] = p_value;
    }
    _FORCE_INLINE_ const Variant& get_register(Register p_reg) const {
        return registers[p_reg];
    }

    void set_property(Register p_reg, const Variant& p_value);
    Variant get_property(Register p_reg) const;

    void reset(Pattern* p_owner, int p_local_id);

    Pattern* get_pattern() const;
    Danmaku* get_danmaku() const;

    void set_global_position(const Vector2& p_global_position);
    Vector2 get_global_position() const;

    void set_sprite(const String& p_key);
    String get_sprite() const;

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

    Shot();
};

#endif