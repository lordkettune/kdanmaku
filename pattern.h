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

#define MAX_EFFECTS 32

// Note that this extends Node2D, and therefore has a transform.
// Shots will respect this transform. Therefore, if you rotate, move, or scale a Pattern, its Shots will move 
// respectively. This can be useful for advanced patterns in which a group of shots move together.
class Pattern : public Node2D {
    GDCLASS(Pattern, Node2D);

    Dictionary parameters;     // Firing parameters
    Ref<Reference> delegate;   // Any object set by the user that has methods for custom patterns, actions, and selectors
    float despawn_distance;    // Extra distance outside of Danmaku region where shot will despawn
    bool autodelete;           // Pattern will delete itself once all of its shots have despawned

public:
    Danmaku* get_danmaku();

    ShotEffect* make_effect(int p_id);

    void single(Dictionary p_override);
    void circle(int p_count, Dictionary p_override);
    void fan(int p_count, float p_theta, Dictionary p_override);
    void layered(int p_layers, float p_min, float p_max, Dictionary p_override);
    void layered_circle(int p_count, int p_layers, float p_min, float p_max, Dictionary p_override);
    void layered_fan(int p_count, float p_theta, int p_layers, float p_min, float p_max, Dictionary p_override);
    void custom(int p_count, String p_name, Dictionary p_override);

    template <typename F>
    void clear(F p_constraint);

    void _enter_tree();
    void _exit_tree();
    void _physics_process(float p_delta);
    void _draw();

    static void _register_methods();
    void _init();
    ~Pattern();

private:
    Danmaku* danmaku;                 // Parent Danmaku object
    Vector<Shot*> shots;              // Shots owned by this Pattern
    ShotEffect* effects[MAX_EFFECTS]; // Registered shot effects
    bool has_effects;
    
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
    for (Shot* shot : shots) {
        if (p_constraint(shot)) {
            shot->unflag(Shot::FLAG_ACTIVE);
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
    if (danmaku == nullptr) {
        ERR_PRINT("Pattern is not a descendent of a Danmaku node!");
        return;
    }

    int sprite_id = danmaku->get_sprite_id(param<String>("sprite", p_override, ""));
    Ref<ShotSprite> sprite = danmaku->get_sprite(sprite_id);
    unsigned int effects = ShotEffect::bitmask(param<Array>("effects", p_override, Array()));

    Vector2 offset      = param<Vector2>("offset",   p_override, Vector2(0, 0));
    Vector2 base_offset = param<Vector2>("__offset", p_override, Vector2(0, 0));

    float rotation      = param<float>("rotation",   p_override, 0);
    float base_rotation = param<float>("__rotation", p_override, 0);

    float speed         = param<float>("speed", p_override, 0);

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
        shot->flags = Shot::FLAG_ACTIVE;
        shot->effects = effects;
        shot->time = 0;
        shot->owner = this;
        shot->local_id = i;
        shot->sprite_id = sprite_id;
        shot->radius = sprite->collider_radius;
        shot->position = offset;
        shot->speed = speed;
        shot->direction = direction;
        p_callback(shot);
        shots.push_back(shot);
    }
}

#endif