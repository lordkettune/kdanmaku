#ifndef __KD_DANMAKU_HPP__
#define __KD_DANMAKU_HPP__

#include <Godot.hpp>
#include <Node2D.hpp>

#include "shot_sprite.hpp"
#include "shot.hpp"

namespace godot {

class Hitbox;

class Danmaku : public Node2D {
    GODOT_CLASS(Danmaku, Node2D)

private:
    Shot** _free_shots;
    int _free_count;
    int _max_shots;

    int _active_count;
    int _pattern_count;

    ShotSprite** _sprites;
    int _sprite_count;

    Hitbox* _hitbox;
    
public:
    Rect2 region;
    int tolerance;

    // Public versions of _max_shots and _sprites.
    // These can't be changed after initialization without breaking things, so they're copied to a private version in _enter_tree
    int max_shots;
    Array sprites;

    Vector2 clear_origin;
    float clear_radius;
    bool clear_enabled;

    static void _register_methods();
    void _init() {};
    bool is_danmaku();

    Danmaku();
    ~Danmaku();

    void _enter_tree();
    void _exit_tree();

    void count_pattern() { ++_pattern_count; }
    void decount_pattern() { --_pattern_count; }

    void register_hitbox(Hitbox* hitbox) { _hitbox = hitbox; }
    void remove_hitbox() { _hitbox = nullptr; }
    Hitbox* get_hitbox() { return _hitbox; }

    void capture(Shot** buf, int count);
    void release(Shot** buf, int count);

    bool should_clear(Vector2 position);

    inline ShotSprite* get_sprite(int id) { return _sprites[id]; }
    int get_sprite_id(const String& key);

    inline Rect2 get_region()  { return region;    }
    inline int get_tolerance() { return tolerance; }

    int get_free_shot_count()   { return _free_count;    }
    int get_active_shot_count() { return _active_count;  }
    int get_pattern_count()     { return _pattern_count; }
};

};

#endif