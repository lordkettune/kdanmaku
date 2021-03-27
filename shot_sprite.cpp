#include "shot_sprite.h"

void ShotSprite::set_key(const String& p_key) {
    key = p_key;
}

String ShotSprite::get_key() const {
    return key;
}

void ShotSprite::set_region(const Rect2& p_region) {
    region = p_region;
}

Rect2 ShotSprite::get_region() const {
    return region;
}

void ShotSprite::set_collider_radius(float p_collider_radius) {
    collider_radius = p_collider_radius;
}

float ShotSprite::get_collider_radius() const {
    return collider_radius;
}

void ShotSprite::set_rotation_degrees(float p_rotation_degrees) {
    rotation_degrees = p_rotation_degrees;
}

float ShotSprite::get_rotation_degrees() const {
    return rotation_degrees;
}

void ShotSprite::set_face_motion(bool p_face_motion) {
    face_motion = p_face_motion;
}

bool ShotSprite::get_face_motion() const {
    return face_motion;
}

void ShotSprite::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_key", "key"), &ShotSprite::set_key);
    ClassDB::bind_method(D_METHOD("set_region", "region"), &ShotSprite::set_region);
    ClassDB::bind_method(D_METHOD("set_collider_radius", "collider_radius"), &ShotSprite::set_collider_radius);
    ClassDB::bind_method(D_METHOD("set_rotation_degrees", "rotation_degrees"), &ShotSprite::set_rotation_degrees);
    ClassDB::bind_method(D_METHOD("set_face_motion", "face_motion"), &ShotSprite::set_face_motion);

    ClassDB::bind_method(D_METHOD("get_key"), &ShotSprite::get_key);
    ClassDB::bind_method(D_METHOD("get_region"), &ShotSprite::get_region);
    ClassDB::bind_method(D_METHOD("get_collider_radius"), &ShotSprite::get_collider_radius);
    ClassDB::bind_method(D_METHOD("get_rotation_degrees"), &ShotSprite::get_rotation_degrees);
    ClassDB::bind_method(D_METHOD("get_face_motion"), &ShotSprite::get_face_motion);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "key"), "set_key", "get_key");
    ADD_PROPERTY(PropertyInfo(Variant::RECT2, "region"), "set_region", "get_region");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "collider_radius"), "set_collider_radius", "get_collider_radius");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "rotation_degrees"), "set_rotation_degrees", "get_rotation_degrees");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "face_motion"), "set_face_motion", "get_face_motion");
}

ShotSprite::ShotSprite() {
    key = "";
    region = Rect2(0, 0, 0, 0);
    collider_radius = 16;
    rotation_degrees = 0;
    face_motion = false;
}