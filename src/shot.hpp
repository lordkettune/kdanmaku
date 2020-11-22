#ifndef __KD_SHOT_HPP__
#define __KD_SHOT_HPP__

// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// *:･ﾟ✧ shot.hpp *:･ﾟ✧
// 
// Shot object. This doesn't really do much on its own; it's mostly just a data object.
// Managed by Danmaku and Pattern nodes.
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

#include <Godot.hpp>
#include <Object.hpp>

namespace godot {

class Pattern;
class Danmaku;

class Shot : public Object {
    GODOT_CLASS(Shot, Object)

public:
    int global_id;
    int local_id;

    bool is_grazing;
    bool is_colliding;

    Pattern* owner;
    int time;
    bool active;
    int sprite_id;
    Vector2 global_position;
    
    Vector2 position;
    Vector2 direction;
    float speed;
    float radius;

    void reset();

    Pattern* get_pattern();
    Danmaku* get_danmaku();

    int get_time();

    void set_sprite(String p_key);
    String get_sprite();

    void set_velocity(Vector2 p_velocity);
    Vector2 get_velocity();

    void set_rotation(float p_rotation);
    float get_rotation();

    static void _register_methods();
    void _init();
};

}

#endif