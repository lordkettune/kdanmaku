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

public:
    static void _register_methods();
    void _init() {};

    Pattern();
    ~Pattern();

    void _enter_tree();
    void _exit_tree();
    void _physics_process(float delta);
    void _draw();
    
    void fire_circle(String sprite, int count, float speed);
};

};

#endif