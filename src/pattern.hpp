#ifndef __KD_PATTERN_HPP__
#define __KD_PATTERN_HPP__

#include <Godot.hpp>
#include <Node2D.hpp>

#include "danmaku.hpp"

namespace godot {

class Pattern : public Node2D {
    GODOT_CLASS(Pattern, Node2D)

private:
    Danmaku* _danmaku;

    int* _shot_ids;
    int _shot_ids_size;
    int _shot_count;

    int* buffer(int count);
    bool prepare(const String& sprite, int& sprite_id, float& radius, float& angle, int count, bool aim, int*& buf);

public:
    static void _register_methods();
    void _init() {};

    Pattern();
    ~Pattern();

    void _enter_tree();
    void _exit_tree();
    void _physics_process(float delta);
    void _draw();
    
    void fire(String sprite, float speed, float angle, bool aim);
    void fire_layered(String sprite, int layers, float min_speed, float max_speed, float angle, bool aim);
    void fire_circle(String sprite, int count, float speed, float angle, bool aim);
    void fire_layered_circle(String sprite, int count, int layers, float min_speed, float max_speed, float angle, bool aim);
    void fire_fan(String sprite, int count, float speed, float angle, float theta, bool aim);
    void fire_layered_fan(String sprite, int count, int layers, float min_speed, float max_speed, float angle, float theta, bool aim);
};

};

#endif