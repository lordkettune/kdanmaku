#ifndef __KD_PATTERN_HPP__
#define __KD_PATTERN_HPP__

// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// *:･ﾟ✧ pattern.hpp *:･ﾟ✧
// 
// Pattern is the main object for this library -- all shot firing functionality is here.
// Each Pattern keeps its own list of Shots that it owns, which it returns to its parent Danmaku
// node upon deletion or when shots are cleared.
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

#include <Godot.hpp>
#include <Node2D.hpp>
#include <Reference.hpp>

#include "utils.hpp"
#include "danmaku.hpp"
#include "shot.hpp"

namespace godot {

// Note that this extends Node2D, and therefore has a transform.
// Shots will respect this transform. Therefore, if you rotate, move, or scale a Pattern, its Shots will move 
// respectively. This can be useful for advanced patterns in which a group of shots move together.
class Pattern : public Node2D {
    GODOT_CLASS(Pattern, Node2D)

    Dictionary parameters;        // Firing parameters
    Ref<Reference> delegate;      // Any object set by the user that has methods for custom patterns, actions, and selectors

public:
    Danmaku* get_danmaku();

    void single(Dictionary p_override);
    void circle(int p_count, Dictionary p_override);
    void fan(int p_count, float p_theta, Dictionary p_override);
    void layered(int p_layers, float p_min, float p_max, Dictionary p_override);
    void layered_circle(int p_count, int p_layers, float p_min, float p_max, Dictionary p_override);
    void layered_fan(int p_count, float p_theta, int p_layers, float p_min, float p_max, Dictionary p_override);

    // Fires a custom pattern.
    // Custom patterns are implemented via the pattern's delegate.
    // The delegate method should be in the format:
    //    func custom(count, index, shot):
    //        pass
    // This method will be called for every shot fired, at which point you should set the speed, direction,
    // and whatever other parameters you want to initialize.
    void custom(int p_count, String p_name, Dictionary p_override);

    template <typename F>
    void clear(F p_constraint);

    void _enter_tree();
    void _exit_tree();
    void _physics_process(float p_delta);
    void _draw();

    static void _register_methods();
    void _init();

private:
    Danmaku* danmaku;         // Parent Danmaku object
    Vector<Shot*> shots;      // Shots owned by this Pattern
    
    template <typename T>
    T param(String p_key, const Dictionary& p_override, T p_default);

    template <typename F>
    void pattern(int p_count, const Dictionary& p_override, F p_callback);
};

// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// Template method implementations
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

template <typename F>
void Pattern::clear(F p_constraint) {
    for (Shot* shot : shots) {
        if (p_constraint(shot)) {
            shot->active = false;
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

template <typename F>
void Pattern::pattern(int p_count, const Dictionary& p_override, F p_callback) {
    if (danmaku == nullptr) {
        ERR_PRINT("Pattern is not a descendent of a Danmaku node!");
        return;
    }

    int sprite_id = danmaku->get_sprite_id(param<String>("sprite", p_override, ""));
    Ref<ShotSprite> sprite = danmaku->get_sprite(sprite_id);

    Vector2 offset = param<Vector2>("offset", p_override, Vector2(0, 0));
    float rotation = param<float>("rotation", p_override, 0);
    float speed = param<float>("speed", p_override, 0);

    Vector2 direction = Vector2(1, 0);
    if (param<bool>("aim", p_override, false)) {
        direction = danmaku->get_hitbox()->get_global_position() - get_global_position();
        direction.normalize();
    }
    direction = direction.rotated(rotation);

    for (int i = 0; i != p_count; ++i) {
        Shot* shot = danmaku->capture();
        shot->time = 0;
        shot->owner = this;
        shot->local_id = i;
        shot->sprite_id = sprite_id;
        shot->radius = sprite->collider_radius;
        shot->position = offset;
        shot->speed = speed;
        shot->direction = direction;
        shot->is_grazing = false;
        shot->is_colliding = false;
        shot->active = true;
        p_callback(shot);
        shots.push_back(shot);
    }
}

};

#endif