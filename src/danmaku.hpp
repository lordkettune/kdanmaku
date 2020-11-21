#ifndef __KD_DANMAKU_HPP__
#define __KD_DANMAKU_HPP__

// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// *:･ﾟ✧ danmaku.hpp *:･ﾟ✧
// 
// Danmaku is a node that acts similarly to Environment or Navigation.
// It's a manager object that Hitboxes and Patterns need to be a descendent of in the scene tree.
//
// Danmaku has several functions:
//     1. Pool shots. Upon scene load, create a fixed size array of shots, and allocate them
//        to Patterns as needed.
//     2. Manage shot sprites. Any shot sprites a game will use need to be registered here,
//        so they can be accessed during gameplay via their key.
//     3. Defines gameplay region and clear circle -- Patterns will despawn shots that leave
//        the gameplay region, and clear shots that are inside the clear circle.
//     4. Keep track of the player's Hitbox.
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

#include <Godot.hpp>
#include <Node2D.hpp>

#include <vector>

#include "shot_sprite.hpp"
#include "shot.hpp"
#include "utils.hpp"

namespace godot {

class Hitbox;

// TODO: Maybe this shouldn't extend from Node2D since it operates in global coordinates anyway?
class Danmaku : public Node2D {
    GODOT_CLASS(Danmaku, Node2D)

private:
    // List of shots that aren't owned by Patterns
    Shot** _free_shots;
    int _free_count;
    int _max_shots;

    int _active_count;            // Active shot count
    int _pattern_count;           // Active pattern count
    Vector<ShotSprite*> _sprites; // Registered shot sprites
    Hitbox* _hitbox;              // The player
    
public:
    // Public versions of _max_shots and _sprites.
    // These can't be changed after initialization without breaking things, so they're copied to a private version in _enter_tree
    int max_shots;
    Array sprites;

    Rect2 region;                 // Gameplay rectangle -- note that this is in global coordinates!
    int tolerance;                // Distance outside of gameplay rect where shots will despawn
    
    Vector2 clear_origin;         // Clear circle -- also global coordinates
    float clear_radius;
    bool clear_enabled;

    static void _register_methods();
    void _init() {};
    bool is_danmaku();

    Danmaku();
    ~Danmaku();

    void _enter_tree();
    void _exit_tree();

    // Debug counter for patterns
    void count_pattern()   { ++_pattern_count; }
    void decount_pattern() { --_pattern_count; }

    // Hitbox management
    void register_hitbox(Hitbox* hitbox) { _hitbox = hitbox; }
    void remove_hitbox() { _hitbox = nullptr; }
    Hitbox* get_hitbox() { return _hitbox; }

    // Gets the next free shot, and removes it from the free list.
    Shot* capture();

    // Puts a shot back into the free list.
    void release(Shot* shot);

    // Checks if a global position is in the clear circle.
    bool should_clear(Vector2 position);

    // Shot sprite management
    inline ShotSprite* get_sprite(int id) { return _sprites[id]; }
    int get_sprite_id(const String& key);

    // Debug counts
    int get_free_shot_count()   { return _free_count;    }
    int get_active_shot_count() { return _active_count;  }
    int get_pattern_count()     { return _pattern_count; }
};

};

#endif