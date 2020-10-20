#ifndef __KD_PATTERN_HPP__
#define __KD_PATTERN_HPP__

#include <Godot.hpp>
#include <Node2D.hpp>
#include <Reference.hpp>

#include "utils.hpp"
#include "danmaku.hpp"
#include "shot.hpp"
#include "selector.hpp"
#include "action.hpp"

namespace godot {

class Pattern : public Node2D {
    GODOT_CLASS(Pattern, Node2D)

private:
    struct Mapping {
        ISelector* selector;
        IAction* action;
    };

    Danmaku* _danmaku;
    int _current_local_id;

    Vector<Shot*> _shots;
    Vector<Mapping> _mappings;

    int _fire_sprite;
    float _fire_radius;
    Vector2 _fire_direction;

    bool validate_fire();
    Shot* next_shot();

    ISelector* make_selector(String source);
    IAction* make_action(String source);

public:
    Vector2 fire_offset;
    float fire_speed;

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
    
    void fire();
    void fire_circle(int count);
    void fire_fan(int count, float theta);
    void fire_layered(int layers, float min, float max);
    void fire_layered_circle(int count, int layers, float min, float max);
    void fire_layered_fan(int count, float theta, int layers, float min, float max);
    void fire_custom(int count, String name);

    Array select(String selector);
    void apply(String action);
    void map(String selector_source, String action_source);
};

};

#endif