// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// *:･ﾟ✧ pattern.hpp *:･ﾟ✧
// 
// Pattern is the main object for this library -- all shot firing functionality is here.
// Each Pattern keeps its own list of Shots that it owns, which it returns to its parent Danmaku
// node upon deletion or when shots are cleared.
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

#ifndef PATTERN_H
#define PATTERN_H

#include "scene/2d/node_2d.h"

#include "danmaku.h"
#include "shot.h"
#include "shot_effect.h"
#include "shot_sprite.h"

#define PATTERN_REGISTERS 8 + 4
#define PATTERN_REG(idx) (REG_PATTERN | (idx << 2))

class Pattern : public Node2D {
    GDCLASS(Pattern, Node2D);

    Danmaku* danmaku;
    Vector<Shot*> shots;
    Ref<Reference> delegate;

    struct {
        int count;
        String shape;
        String sprite;
        Vector2 offset;
        Ref<ShotEffect> effect;
        float rotation;
        float speed;
        bool paused;
        bool aim;
    } fire_params;

    int effect_count;
    Variant registers[PATTERN_REGISTERS];

    float despawn_distance;
    bool autodelete;
    uint32_t collision_layers;

protected:
    void _notification(int p_what);
    static void _bind_methods();

public:
    enum {
        REG0 = PATTERN_REG(0),
        REG1 = PATTERN_REG(1),
        REG2 = PATTERN_REG(2),
        REG3 = PATTERN_REG(3),
        REG4 = PATTERN_REG(4),
        REG5 = PATTERN_REG(5),
        REG6 = PATTERN_REG(6),
        REG7 = PATTERN_REG(7),

        FIRE_SHAPE0 = PATTERN_REG(8),
        FIRE_SHAPE1 = PATTERN_REG(9),
        FIRE_SHAPE2 = PATTERN_REG(10),
        FIRE_SHAPE3 = PATTERN_REG(11),

        FIRE_COUNT    = PATTERN_REG(12),
        FIRE_SHAPE    = PATTERN_REG(13),
        FIRE_SPRITE   = PATTERN_REG(14),
        FIRE_OFFSET   = PATTERN_REG(15),
        FIRE_EFFECT   = PATTERN_REG(16),
        FIRE_ROTATION = PATTERN_REG(17),
        FIRE_SPEED    = PATTERN_REG(18),
        FIRE_PAUSED   = PATTERN_REG(19),
        FIRE_AIM      = PATTERN_REG(20)
    };

    void set_register(Register p_reg, const Variant& p_value);
    Variant get_register(Register p_reg) const;

    void set_fire_count(int p_count);
    int get_fire_count() const;

    void set_fire_shape(String p_shape);
    String get_fire_shape() const;

    void set_fire_sprite(String p_sprite);
    String get_fire_sprite() const;

    void set_fire_offset(Vector2 p_offset);
    Vector2 get_fire_offset() const;

    void set_fire_effect(Ref<ShotEffect> p_effect);
    Ref<ShotEffect> get_fire_effect() const;

    void set_fire_rotation(float p_rotation);
    float get_fire_rotation() const;

    void set_fire_speed(float p_speed);
    float get_fire_speed() const;

    void set_fire_paused(bool p_paused);
    bool get_fire_paused() const;

    void set_fire_aim(bool p_aim);
    bool get_fire_aim() const;

    void fire();
    void fire_single();
    void fire_circle();
    void fire_fan(float p_angle);
    void fire_single_layered(float p_step);
    void fire_circle_layered(int p_layers, float p_step);
    void fire_fan_layered(float p_angle, int p_layers, float p_step);
    void fire_custom(String p_name, Variant p_s0, Variant p_s1, Variant p_s2, Variant p_s3);

    void reset();

    void shape_single(Shot* p_shot);
    void shape_circle(Shot* p_shot);
    void shape_fan(Shot* p_shot);
    void shape_single_layered(Shot* p_shot);
    void shape_circle_layered(Shot* p_shot);
    void shape_fan_layered(Shot* p_shot);
    void shape_custom(Shot* p_shot);

    template <typename F>
    void clear(F p_constraint);

    void play_sfx(const StringName& p_key);

    Danmaku* get_danmaku() const;
    void remove_from_danmaku();

    void set_delegate(Ref<Reference> p_delegate);
    Ref<Reference> get_delegate() const;

    void set_despawn_distance(float p_despawn_distance);
    float get_despawn_distance() const;

    void set_autodelete(bool p_autodelete);
    bool get_autodelete() const;

    void set_collision_layers(uint32_t p_collision_layers);
    uint32_t get_collision_layers() const;

    int fill_buffer(real_t*& buf);

    Pattern();

private:
    void _tick();
};

// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// Template method implementations
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

template <typename Fn>
void Pattern::clear(Fn p_constraint) {
    for (int i = 0; i != shots.size(); ++i) {
        if (p_constraint(shots[i])) {
            shots[i]->clear();
        }
    }
}

#endif