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
#include "scene/resources/texture.h"

#include "shot_sprite.h"
#include "shot.h"

class Hitbox;
class Pattern;

class Danmaku : public Node2D {
    GDCLASS(Danmaku, Node2D);

    Rect2 region;                 
    float tolerance;
    
    int max_shots;
    Vector<Shot*> free_shots;
    Vector<Pattern*> patterns;
    Hitbox* hitbox;

    Vector<Ref<ShotSprite>> sprites;
    Ref<Texture> atlas;

    PoolRealArray buffer;
    RID multimesh;
    RID mesh;
    RID material;
    RID shader;

protected:
    void _notification(int p_what);
    static void _bind_methods();
    virtual void _validate_property(PropertyInfo& property) const;

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

    Ref<ShotSprite> get_sprite(const String& p_key) const;

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

    void set_atlas(const Ref<Texture>& p_atlas);
    Ref<Texture> get_atlas() const;

    void _update_buffer();

    Danmaku();
    ~Danmaku();

private:
    void _create_mesh();
    void _create_material();
};

#endif