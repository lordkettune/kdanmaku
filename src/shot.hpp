#ifndef __KD_SHOT_HPP__
#define __KD_SHOT_HPP__

#include <Godot.hpp>
#include <Object.hpp>

namespace godot {

class Shot : public Object {
    GODOT_CLASS(Shot, Object)

public:
    static void _register_methods();
    void _init();

    bool active;
    Vector2 global_position;
    Vector2 position;
    Vector2 direction;
    float speed;
    float radius;
    int sprite_id;
};

}

#endif