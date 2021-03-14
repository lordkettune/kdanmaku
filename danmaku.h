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

#ifndef DANMAKU_H
#define DANMAKU_H

#include "scene/2d/node_2d.h"

#include "shot_sprite.h"
#include "shot.h"

class Hitbox;
class Pattern;

class Danmaku : public Node2D {
    GDCLASS(Danmaku, Node2D);

    // Gameplay rectangle -- note that this is in global coordinates!
    Rect2 region;                 
    float tolerance;
    
    // Registered shot sprites
    Vector<Ref<ShotSprite>> sprites;
    
    // Shot pool size
    int max_shots;

    // Shots not owned by patterns
    Vector<Shot*> free_shots;

    // Active patterns
    Vector<Pattern*> patterns;
    
    // Player hitbox
    Hitbox* hitbox;

protected:
    void _notification(int p_what);
    static void _bind_methods();

public:
    void add_pattern(Pattern* p_pattern);
    void remove_pattern(Pattern* p_pattern);

    void add_hitbox(Hitbox* p_hitbox);
    void remove_hitbox();
    Hitbox* get_hitbox() const;

    Shot* capture();
    void release(Shot* p_shot);

    void clear_circle(Vector2 p_origin, float p_radius);
    void clear_rect(Rect2 p_rect);

    int get_sprite_id(const StringName& p_key) const;
    Ref<ShotSprite> get_sprite(int p_id) const;

    int get_free_shot_count() const;
    int get_active_shot_count() const;
    int get_pattern_count() const;

    void set_region(const Rect2& p_region);
    Rect2 get_region() const;

    void set_tolerance(float p_tolerance);
    float get_tolerance() const;

    void set_max_shots(int p_max_shots);
    int get_max_shots() const;

    void set_shot_sprite_count(int p_count);
    int get_shot_sprite_count() const;

    void set_shot_sprite(int p_index, const Ref<ShotSprite>& p_sprite);
    Ref<ShotSprite> get_shot_sprite(int p_index) const;

    Danmaku();
};

#endif