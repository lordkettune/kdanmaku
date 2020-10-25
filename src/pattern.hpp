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

private:
    // Selector -> Action mapping
    struct Mapping {
        ISelector* selector;
        IAction* action;
    };

    Danmaku* _danmaku;
    int _current_local_id;

    // Shots owned by this Pattern
    Vector<Shot*> _shots;

    // List of Selectors to test each frame, mapped to actions that should be applied if passed
    Vector<Mapping> _mappings;

    // Sprite to use on next fire, and radius from that sprite
    int _fire_sprite;
    float _fire_radius;

    // Direction to use on next fire
    Vector2 _fire_direction;

    bool validate_fire();
    Shot* next_shot();

    ISelector* make_selector(String source);
    IAction* make_action(String source);

public:
    // Offset from local 0,0 to use on next fire
    Vector2 fire_offset;

    // Speed to use on next fire
    float fire_speed;

    // Any object set by the user that has methods for custom patterns, actions, and selectors
    Ref<Reference> delegate;

    static void _register_methods();
    void _init() {};

    Pattern();
    ~Pattern();

    void _enter_tree();
    void _exit_tree();
    void _physics_process(float delta);
    void _draw();

    Danmaku* get_danmaku() { return _danmaku; }

    void set_fire_angle(float angle);
    float get_fire_angle();

    void set_fire_sprite(String sprite);
    String get_fire_sprite();
    
    // These are a couple of basic, built-in patterns for convenience
    void fire();
    void fire_circle(int count);
    void fire_fan(int count, float theta);
    void fire_layered(int layers, float min, float max);
    void fire_layered_circle(int count, int layers, float min, float max);
    void fire_layered_fan(int count, float theta, int layers, float min, float max);

    // Fires a custom pattern.
    // Custom patterns are implemented via the pattern's delegate.
    // The delegate method should be in the format:
    //    func custom(count, index, shot):
    //        pass
    // This method will be called for every shot fired, at which point you should set the speed, direction,
    // and whatever other parameters you want to initialize.
    void fire_custom(int count, String name);

    // Filters by a selector, and returns an array of shots that pass. See selector.hpp
    Array select(String selector);

    // Applies an action to all shots. See action.hpp
    void apply(String action);

    // Adds a mapping of a selector to an action which will be applied each frame.
    void map(String selector_source, String action_source);
};

};

#endif