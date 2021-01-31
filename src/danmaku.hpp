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

    // These can't be changed after initialization without breaking things, so they're copied to a private version in _enter_tree
    int max_shots;
    Array registered_sprites;

    Rect2 region;                 // Gameplay rectangle -- note that this is in global coordinates!
    int tolerance;                // Distance outside of gameplay rect where shots will despawn
    
    Vector2 clear_origin;         // Clear circle -- also global coordinates
    float clear_radius;
    bool clear_enabled;
    
public:
    void count_pattern();
    void decount_pattern();

    void register_hitbox(Hitbox* p_hitbox);
    void remove_hitbox();
    Hitbox* get_hitbox();

    Shot* capture();
    void release(Shot* p_shot);

    Rect2 get_region();

    bool should_clear(Vector2 p_position);

    Ref<ShotSprite> get_sprite(int p_id);
    int get_sprite_id(const String& p_key);

    int get_free_shot_count();
    int get_active_shot_count();
    int get_pattern_count();

    bool is_danmaku();

    void _enter_tree();
    void _exit_tree();
    
    static void _register_methods();
    void _init();

private:
    Vector<Shot*> free_shots;    // Shots not owned by patterns
    Vector<Ref<ShotSprite>> sprites; // Registered shot sprites
    Hitbox* hitbox;              // The player
    int active_shots;            // Active shot count
    int active_patterns;         // Active pattern count
};

};

#endif