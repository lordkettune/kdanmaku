// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// *:･ﾟ✧ shot_sprite.hpp *:･ﾟ✧
// 
// Resource type that controls how a pattern renders its shots, as well as their collider sizes.
// These should be registered to a Danmaku node, and will be referred to by their keys.
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

#ifndef SHOT_SPRITE_H
#define SHOT_SPRITE_H

#include "scene/resources/texture.h"

struct ShotFrame {
    int next;
    int delay;
    float radius;
    bool face_motion;
    Rect2 region;
};

class ShotSprite : public Resource {
    GDCLASS(ShotSprite, Resource);

    String key;
    float collider_radius;
    bool face_motion;

    Rect2 region;
    int x_frames;
    int y_frames;
    int frame_delay;

    Ref<ShotSprite> spawn_sprite;
    Ref<ShotSprite> clear_sprite;

protected:
    static void _bind_methods();

public:
    void set_key(const String& p_key);
    String get_key() const;

    void set_collider_radius(float p_collider_radius);
    float get_collider_radius() const;

    void set_face_motion(bool p_face_motion);
    bool get_face_motion() const;

    void set_region(const Rect2& p_region);
    Rect2 get_region() const;

    void set_x_frames(int p_frames);
    int get_x_frames() const;

    void set_y_frames(int p_frames);
    int get_y_frames() const;

    void set_frame_delay(int p_delay);
    int get_frame_delay() const;

    void set_spawn_sprite(const Ref<ShotSprite>& p_sprite);
    Ref<ShotSprite> get_spawn_sprite() const;

    void set_clear_sprite(const Ref<ShotSprite>& p_sprite);
    Ref<ShotSprite> get_clear_sprite() const;

    ShotFrame get_frame(int p_id) const;

    ShotSprite();
};

#endif