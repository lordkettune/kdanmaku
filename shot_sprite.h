// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// *:･ﾟ✧ shot_sprite.hpp *:･ﾟ✧
// 
// Resource type that controls how a pattern renders its shots, as well as their collider sizes.
// These should be registered to a Danmaku node, and will be referred to by their keys.
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

#ifndef SHOT_SPRITE_H
#define SHOT_SPRITE_H

#include "scene/resources/texture.h"

class ShotSprite : public Resource {
    GDCLASS(ShotSprite, Resource);

    StringName key;          // Key that this will be referred to by during gameplay

    Ref<Texture> texture;    // Sprite sheet that this is in -- it's a good idea to keep your bullet sprites in an atlas for performance
    Rect2 region;            // Texture region to render
    
    float collider_radius;   // Collider radius shots with this sprite should use

    float rotation_degrees;  // Base rotation
    bool face_motion;        // Sprite will turn to face movement vector

protected:
    static void _bind_methods();

public:
    void set_key(const StringName& p_key);
    StringName get_key() const;

    void set_texture(const Ref<Texture>& p_texture);
    Ref<Texture> get_texture() const;

    void set_region(const Rect2& p_region);
    Rect2 get_region() const;

    void set_collider_radius(float p_collider_radius);
    float get_collider_radius() const;

    void set_rotation_degrees(float p_rotation_degrees);
    float get_rotation_degrees() const;

    void set_face_motion(bool p_face_motion);
    bool get_face_motion() const;

    ShotSprite();
};

#endif