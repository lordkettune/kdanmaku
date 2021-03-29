#include "shot_sprite.h"

void ShotSprite::set_key(const String& p_key) {
    key = p_key;
}

String ShotSprite::get_key() const {
    return key;
}

void ShotSprite::set_collider_radius(float p_collider_radius) {
    collider_radius = p_collider_radius;
}

float ShotSprite::get_collider_radius() const {
    return collider_radius;
}

void ShotSprite::set_face_motion(bool p_face_motion) {
    face_motion = p_face_motion;
}

bool ShotSprite::get_face_motion() const {
    return face_motion;
}

void ShotSprite::set_region(const Rect2& p_region) {
    region = p_region;
}

Rect2 ShotSprite::get_region() const {
    return region;
}

void ShotSprite::set_x_frames(int p_frames) {
    ERR_FAIL_COND(p_frames < 1);
    x_frames = p_frames;
}

int ShotSprite::get_x_frames() const {
    return x_frames;
}

void ShotSprite::set_y_frames(int p_frames) {
    ERR_FAIL_COND(p_frames < 1);
    y_frames = p_frames;
}

int ShotSprite::get_y_frames() const {
    return y_frames;
}

void ShotSprite::set_frame_delay(int p_delay) {
    ERR_FAIL_COND(p_delay < 1);
    frame_delay = p_delay;
}

int ShotSprite::get_frame_delay() const {
    return frame_delay;
}

void ShotSprite::set_spawn_sprite(const Ref<ShotSprite>& p_sprite) {
    spawn_sprite = p_sprite;
}

Ref<ShotSprite> ShotSprite::get_spawn_sprite() const {
    return spawn_sprite;
}

void ShotSprite::set_clear_sprite(const Ref<ShotSprite>& p_sprite) {
    clear_sprite = p_sprite;
}

Ref<ShotSprite> ShotSprite::get_clear_sprite() const {
    return clear_sprite;
}

ShotFrame ShotSprite::get_frame(int p_id) const {
    Rect2 r = region;
    r.size.width /= x_frames;
    r.size.height /= y_frames;
    r.position.x += r.size.width * (p_id % x_frames);
    r.position.y += r.size.height * (p_id / x_frames);

    ShotFrame result;
    result.region = r;
    result.delay = frame_delay;
    result.face_motion = face_motion;
    result.radius = collider_radius;
    result.next = (p_id + 1) % (x_frames * y_frames);
    return result;
}

void ShotSprite::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_key", "key"), &ShotSprite::set_key);
    ClassDB::bind_method(D_METHOD("set_collider_radius", "collider_radius"), &ShotSprite::set_collider_radius);
    ClassDB::bind_method(D_METHOD("set_face_motion", "face_motion"), &ShotSprite::set_face_motion);
    ClassDB::bind_method(D_METHOD("set_region", "region"), &ShotSprite::set_region);
    ClassDB::bind_method(D_METHOD("set_x_frames", "frames"), &ShotSprite::set_x_frames);
    ClassDB::bind_method(D_METHOD("set_y_frames", "frames"), &ShotSprite::set_y_frames);
    ClassDB::bind_method(D_METHOD("set_frame_delay", "delay"), &ShotSprite::set_frame_delay);
    ClassDB::bind_method(D_METHOD("set_spawn_sprite", "sprite"), &ShotSprite::set_spawn_sprite);
    ClassDB::bind_method(D_METHOD("set_clear_sprite", "sprite"), &ShotSprite::set_clear_sprite);

    ClassDB::bind_method(D_METHOD("get_key"), &ShotSprite::get_key);
    ClassDB::bind_method(D_METHOD("get_collider_radius"), &ShotSprite::get_collider_radius);
    ClassDB::bind_method(D_METHOD("get_face_motion"), &ShotSprite::get_face_motion);
    ClassDB::bind_method(D_METHOD("get_region"), &ShotSprite::get_region);
    ClassDB::bind_method(D_METHOD("get_x_frames"), &ShotSprite::get_x_frames);
    ClassDB::bind_method(D_METHOD("get_y_frames"), &ShotSprite::get_y_frames);
    ClassDB::bind_method(D_METHOD("get_frame_delay"), &ShotSprite::get_frame_delay);
    ClassDB::bind_method(D_METHOD("get_spawn_sprite"), &ShotSprite::get_spawn_sprite);
    ClassDB::bind_method(D_METHOD("get_clear_sprite"), &ShotSprite::get_clear_sprite);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "key"), "set_key", "get_key");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "collider_radius"), "set_collider_radius", "get_collider_radius");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "face_motion"), "set_face_motion", "get_face_motion");
    ADD_PROPERTY(PropertyInfo(Variant::RECT2, "region"), "set_region", "get_region");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "x_frames", PROPERTY_HINT_EXP_RANGE, "1,64,1"), "set_x_frames", "get_x_frames");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "y_frames", PROPERTY_HINT_EXP_RANGE, "1,64,1"), "set_y_frames", "get_y_frames");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "frame_delay", PROPERTY_HINT_EXP_RANGE, "1,64,1"), "set_frame_delay", "get_frame_delay");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "spawn_sprite", PROPERTY_HINT_RESOURCE_TYPE, "ShotSprite"), "set_spawn_sprite", "get_spawn_sprite");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "clear_sprite", PROPERTY_HINT_RESOURCE_TYPE, "ShotSprite"), "set_clear_sprite", "get_clear_sprite");
}

ShotSprite::ShotSprite() {
    key = "";
    collider_radius = 16;
    face_motion = false;

    region = Rect2(0, 0, 0, 0);
    x_frames = 1;
    y_frames = 1;
    frame_delay = 1;

    spawn_sprite = Ref<ShotSprite>();
    clear_sprite = Ref<ShotSprite>();
}