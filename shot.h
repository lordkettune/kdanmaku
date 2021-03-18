// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// *:･ﾟ✧ shot.hpp *:･ﾟ✧
// 
// Shot object. This doesn't really do much on its own; it's mostly just a data object.
// Managed by Danmaku and Pattern nodes.
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

#ifndef SHOT_H
#define SHOT_H

#include "shot_effect.h"
#include "core/object.h"

#define SHOT_REGISTERS 8
#define SHOT_REG(idx) (REG_SHOT | (idx << 2))

class Pattern;
class Danmaku;

class Shot : public Object {
    GDCLASS(Shot, Object);

    int id;
    Pattern* owner;
    uint32_t flags;

    Variant registers[SHOT_REGISTERS];

    int sprite;
    float radius;
    
    Vector2 position;
    Vector2 direction;
    float speed;

protected:
    static void _bind_methods();

public:
    enum {
        REG0      = SHOT_REG(0),
        REG1      = SHOT_REG(1),
        REG2      = SHOT_REG(2),
        REG3      = SHOT_REG(3),
        REG4      = SHOT_REG(4),
        REG5      = SHOT_REG(5),
        REG6      = SHOT_REG(6),
        REG7      = SHOT_REG(7),
        POSITION  = SHOT_REG(8),
        SPEED     = SHOT_REG(9),
        DIRECTION = SHOT_REG(10),
        ROTATION  = SHOT_REG(11),
        VELOCITY  = SHOT_REG(12)
    };

    enum {
        FLAG_ACTIVE    = 1,
        FLAG_GRAZING   = 2,
        FLAG_COLLIDING = 4
    };

    _FORCE_INLINE_ void flag(int p_flag)     { flags |= p_flag;  }
    _FORCE_INLINE_ void unflag(int p_flag)   { flags &= ~p_flag; }
    _FORCE_INLINE_ bool flagged(int p_flag)  { return flags & p_flag; }
    
    _FORCE_INLINE_ void set_sprite_id(int p_sprite) { sprite = p_sprite; }
    _FORCE_INLINE_ int get_sprite_id() { return sprite; }

    _FORCE_INLINE_ int get_id() { return id; }

    void set_register(Register p_reg, const Variant& p_value);
    Variant get_register(Register p_reg) const;

    void reset(Pattern* p_owner, int p_local_id);

    Pattern* get_pattern() const;
    Danmaku* get_danmaku() const;

    void set_sprite(const String& p_key);
    String get_sprite() const;

    void set_radius(float p_radius);
    float get_radius() const;

    void set_position(const Vector2& p_position);
    Vector2 get_position() const;

    void set_speed(float p_speed);
    float get_speed() const;

    void set_direction(const Vector2& p_direction);
    Vector2 get_direction() const;

    void set_rotation(float p_rotation);
    float get_rotation() const;

    void set_velocity(const Vector2& p_velocity);
    Vector2 get_velocity() const;

    Shot();
};

#endif