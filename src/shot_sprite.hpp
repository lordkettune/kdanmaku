#ifndef __KD_SHOT_SPRITE_HPP__
#define __KD_SHOT_SPRITE_HPP__

// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// *:･ﾟ✧ shot_sprite.hpp *:･ﾟ✧
// 
// Resource type that controls how a pattern renders its shots, as well as their collider sizes.
// These should be registered to a Danmaku node, and will be referred to by their keys.
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

#include <Godot.hpp>
#include <Texture.hpp>
#include <Resource.hpp>
#include <CanvasItem.hpp>

namespace godot {

class ShotSprite : public Resource {
    GODOT_CLASS(ShotSprite, Resource)

public:
    String key;              // Key that this will be referred to by during gameplay
    Ref<Texture> texture;    // Sprite sheet that this is in -- it's a good idea to keep your bullet sprites in an atlas for performance
    Rect2 region;            // Texture region to render
    float collider_radius;   // Collider radius shots with this sprite should use

    static void _register_methods();
    void _init();

    void draw_to(CanvasItem* canvas_item, Vector2 position);
};

};

#endif