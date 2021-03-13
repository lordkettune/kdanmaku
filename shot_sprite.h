// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// *:･ﾟ✧ shot_sprite.hpp *:･ﾟ✧
// 
// Resource type that controls how a pattern renders its shots, as well as their collider sizes.
// These should be registered to a Danmaku node, and will be referred to by their keys.
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

#ifndef SHOT_SPRITE_H
#define SHOT_SPRITE_H

#include "core/io/resource.h"
#include "scene/resources/texture.h"

class ShotSprite : public Resource {
    GDCLASS(ShotSprite, Resource);

public:
    String key;              // Key that this will be referred to by during gameplay
    Ref<Texture> texture;    // Sprite sheet that this is in -- it's a good idea to keep your bullet sprites in an atlas for performance
    Rect2 region;            // Texture region to render
    float collider_radius;   // Collider radius shots with this sprite should use
    float rotation_degrees;  // Base rotation
    bool face_motion;        // Sprite will turn to face movement vector

    static void _register_methods();
    void _init();
};

#endif