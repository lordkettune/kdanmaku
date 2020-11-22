#ifndef __KD_PATTERN_HPP__
#define __KD_PATTERN_HPP__

// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// *:･ﾟ✧ pattern.hpp *:･ﾟ✧
// 
// Pattern is the main object for this library -- all shot firing functionality is here.
// Each Pattern keeps its own list of Shots that it owns, which it returns to its parent Danmaku
// node upon deletion or when shots are cleared.
// Advanced shot behavior is done via Selectors and Actions -- see their respective files for more info.
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

#include <Godot.hpp>
#include <Node2D.hpp>
#include <Reference.hpp>

#include "utils.hpp"
#include "danmaku.hpp"
#include "shot.hpp"
#include "selector.hpp"
#include "action.hpp"

namespace godot {

// Note that this extends Node2D, and therefore has a transform.
// Shots will respect this transform. Therefore, if you rotate, move, or scale a Pattern, its Shots will move 
// respectively. This can be useful for advanced patterns in which a group of shots move together.
class Pattern : public Node2D {
    GODOT_CLASS(Pattern, Node2D)

    Vector2 fire_offset;          // Offset from local 0,0 to use on next fire    
    float fire_speed;             // Speed to use on next fire
    Ref<Reference> delegate;      // Any object set by the user that has methods for custom patterns, actions, and selectors

public:
    Danmaku* get_danmaku();

    void set_fire_angle(float p_angle);
    float get_fire_angle();

    void set_fire_sprite(String p_sprite);
    String get_fire_sprite();

    void fire();
    void fire_circle(int p_count);
    void fire_fan(int p_count, float p_theta);
    void fire_layered(int p_layers, float p_min, float p_max);
    void fire_layered_circle(int p_count, int p_layers, float p_min, float p_max);
    void fire_layered_fan(int p_count, float p_theta, int p_layers, float p_min, float p_max);

    // Fires a custom pattern.
    // Custom patterns are implemented via the pattern's delegate.
    // The delegate method should be in the format:
    //    func custom(count, index, shot):
    //        pass
    // This method will be called for every shot fired, at which point you should set the speed, direction,
    // and whatever other parameters you want to initialize.
    void fire_custom(int p_count, String p_name);

    // Filters by a selector, and returns an array of shots that pass. See selector.hpp
    Array select(String p_selector);

    // Applies an action to all shots. See action.hpp
    void apply(String p_action);

    // Adds a mapping of a selector to an action which will be applied each frame.
    void map(String p_selector_source, String p_action_source);

    void _enter_tree();
    void _exit_tree();
    void _physics_process(float p_delta);
    void _draw();

    static void _register_methods();
    void _init();

private:
    struct Mapping {
        ISelector* selector;
        IAction* action;
    };

    int current_local_id;
    Danmaku* danmaku;
    Vector<Shot*> shots;         // Shots owned by this Pattern
    Vector<Mapping> mappings;    // List of Selectors to test each frame, mapped to actions that should be applied if passed
    
    int fire_sprite;             // Sprite to use on next fire
    float fire_radius;           // Collider radius to use on next fire (from sprite)
    Vector2 fire_direction;      // Direction to use on next fire

    bool validate_fire();
    Shot* next_shot();

    ISelector* make_selector(String p_source);
    IAction* make_action(String p_source);
};

};

#endif