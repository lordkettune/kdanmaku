// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// *:･ﾟ✧ pattern.hpp *:･ﾟ✧
// 
// Pattern is the main object for this library -- all shot firing functionality is here.
// Each Pattern keeps its own list of Shots that it owns, which it returns to its parent Danmaku
// node upon deletion or when shots are cleared.
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

#ifndef PATTERN_H
#define PATTERN_H

#define PATTERN_REGISTERS 32

#include "scene/2d/node_2d.h"

#include "danmaku.h"
#include "shot.h"
#include "shot_effect.h"

// Note that this extends Node2D, and therefore has a transform.
// Shots will respect this transform. Therefore, if you rotate, move, or scale a Pattern, its Shots will move 
// respectively. This can be useful for advanced patterns in which a group of shots move together.
class Pattern : public Node2D {
    GDCLASS(Pattern, Node2D);

    Danmaku* danmaku;
    Vector<Shot*> shots;

    Dictionary parameters;
    Object* delegate;

    Ref<ShotEffect> effect;
    Variant registers[PATTERN_REGISTERS];

    float despawn_distance;
    bool autodelete;

protected:
    void _notification(int p_what);
    static void _bind_methods();

public:
    _FORCE_INLINE_ void set_register(Register p_reg, const Variant& p_value) {
        registers[p_reg] = p_value;
    }
    _FORCE_INLINE_ const Variant& get_register(Register p_reg) const {
        return registers[p_reg];
    }

    template <typename F>
    void clear(F p_constraint);

    Danmaku* get_danmaku() const;

    void single(Dictionary p_override);
    void circle(int p_count, Dictionary p_override);
    void fan(int p_count, float p_theta, Dictionary p_override);
    void layered(int p_layers, float p_min, float p_max, Dictionary p_override);
    void layered_circle(int p_count, int p_layers, float p_min, float p_max, Dictionary p_override);
    void layered_fan(int p_count, float p_theta, int p_layers, float p_min, float p_max, Dictionary p_override);
    void custom(int p_count, String p_name, Dictionary p_override);

    void set_effect(const Ref<ShotEffect>& p_effect);
    Ref<ShotEffect> get_effect() const;

    void set_delegate(Object* p_delegate);
    Object* get_delegate() const;

    void set_parameters(const Dictionary& p_parameters);
    Dictionary get_parameters() const;

    void set_despawn_distance(float p_despawn_distance);
    float get_despawn_distance() const;

    void set_autodelete(bool p_autodelete);
    bool get_autodelete() const;

    Pattern();

private:
    void tick();
    void draw();

    template <typename T>
    T param(String p_key, const Dictionary& p_override, T p_default);

    template <typename Fn>
    void pattern(int p_count, const Dictionary& p_override, Fn p_callback);
};

// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// Template method implementations
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

template <typename Fn>
void Pattern::clear(Fn p_constraint) {
    for (int i = 0; i != shots.size(); ++i) {
        if (p_constraint(shots[i])) {
            shots[i]->unflag(Shot::FLAG_ACTIVE);
        }
    }
}

template <typename T>
T Pattern::param(String p_key, const Dictionary& p_override, T p_default) {
    if (p_override.has(p_key)) {
        return (T)p_override[p_key];
    } else if (parameters.has(p_key)) {
        return (T)parameters[p_key];
    } else {
        return p_default;
    }
}

template <typename Fn>
void Pattern::pattern(int p_count, const Dictionary& p_override, Fn p_callback) {
    if (danmaku == NULL) {
        ERR_PRINT("Pattern is not a descendent of a Danmaku node!");
        return;
    }

    int sprite_id = danmaku->get_sprite_id(param<String>("sprite", p_override, ""));
    Ref<ShotSprite> sprite = danmaku->get_sprite(sprite_id);

    Vector2 offset = param<Vector2>("offset", p_override, Vector2(0, 0));
    float rotation = param<float>("rotation", p_override, 0);
    Vector2 base_offset = param<Vector2>("__offset", p_override, Vector2(0, 0));
    float base_rotation = param<float>("__rotation", p_override, 0);

    float speed = param<float>("speed", p_override, 0);

    offset += base_offset;
    rotation += base_rotation;

    Vector2 direction = Vector2(1, 0);
    if (param<bool>("aim", p_override, false)) {
        direction = danmaku->get_hitbox()->get_global_position() - get_global_position();
        direction.normalize();
    }
    direction = direction.rotated(rotation);

    for (int i = 0; i != p_count; ++i) {
        Shot* shot = danmaku->capture();
        shot->reset(this, i);
        shot->set_sprite_id(sprite_id);
        shot->set_radius(sprite->get_collider_radius());
        shot->set_position(offset);
        shot->set_direction(direction);
        shot->set_speed(speed);
        p_callback(shot);
        shots.push_back(shot);
    }
}

#endif