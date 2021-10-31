// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// *:･ﾟ✧ shot.hpp *:･ﾟ✧
// 
// Shot object. This doesn't really do much on its own; it's mostly just a data object.
// Managed by Danmaku and Pattern nodes.
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

#ifndef SHOT_H
#define SHOT_H

#include "shot_sprite.h"
#include "shot_effect.h"
#include "core/object.h"

#define STATE_REGISTERS 8
#define SHOT_REGISTERS 8
#define SHOT_REG(idx) (REG_SHOT | (idx << 2))

class Pattern;
class Danmaku;

class Shot : public Object {
    GDCLASS(Shot, Object);

    int id;
    Pattern* owner;
    uint32_t flags;

    Ref<ShotEffect> effect;
    int instruction_pointers[MAX_SHOT_EFFECTS];
    Variant registers[SHOT_REGISTERS];
    Variant state[STATE_REGISTERS];

    Ref<ShotSprite> sprite;
    ShotFrame frame;
    
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
        VELOCITY  = SHOT_REG(12),
        PAUSED    = SHOT_REG(13),
        SPRITE    = SHOT_REG(14)
    };

    enum {
        FLAG_ACTIVE    = 1,
        FLAG_CLEARED   = 2,
        FLAG_GRAZING   = 4,
        FLAG_COLLIDING = 8,
        FLAG_PAUSED    = 16
    };

    _FORCE_INLINE_ void flag(int p_flag)     { flags |= p_flag;  }
    _FORCE_INLINE_ void unflag(int p_flag)   { flags &= ~p_flag; }
    _FORCE_INLINE_ bool flagged(int p_flag) const { return flags & p_flag; }

    _FORCE_INLINE_ int get_id() { return id; }
    _FORCE_INLINE_ int* get_instruction_pointer(int p_idx) { return &instruction_pointers[p_idx]; }
    _FORCE_INLINE_ float get_radius() { return frame.radius; }
    _FORCE_INLINE_ Variant* get_state() { return state; }
    _FORCE_INLINE_ ShotFrame* get_frame() { return &frame; }

    void reset(Pattern* p_owner, int p_local_id);
    void clear();

    void set_register(Register p_reg, const Variant& p_value);
    Variant get_register(Register p_reg) const;

    void set_effect(Ref<ShotEffect> p_effect);
    Ref<ShotEffect> get_effect() const;

    Pattern* get_pattern() const;
    Danmaku* get_danmaku() const;

    void set_paused(bool p_paused);
    bool get_paused() const;

    void set_sprite(Ref<ShotSprite> p_sprite);
    Ref<ShotSprite> get_sprite() const;

    void set_sprite_key(String p_key);
    String get_sprite_key() const;

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