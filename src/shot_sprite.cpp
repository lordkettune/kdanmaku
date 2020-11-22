#include "shot_sprite.hpp"

using namespace godot;

void ShotSprite::_register_methods() {
    register_property<ShotSprite, String>("key", &ShotSprite::key, "");
    register_property<ShotSprite, Ref<Texture>>("texture", &ShotSprite::texture, nullptr);
    register_property<ShotSprite, Rect2>("region", &ShotSprite::region, Rect2(0, 0, 0, 0));
    register_property<ShotSprite, float>("collider_radius", &ShotSprite::collider_radius, 16);
    register_property<ShotSprite, float>("rotation_degrees", &ShotSprite::rotation_degrees, 0);
    register_property<ShotSprite, bool>("face_motion", &ShotSprite::face_motion, false);
}

void ShotSprite::_init() {
    key = "";
    texture = nullptr;
    region = Rect2(0, 0, 0, 0);
    collider_radius = 16;
    rotation_degrees = 0;
    face_motion = false;
}