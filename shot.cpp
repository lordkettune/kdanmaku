#include "shot.h"
#include "danmaku.h"
#include "pattern.h"

#include <Math.hpp>

Pattern* Shot::get_pattern() {
    return owner;
}

Danmaku* Shot::get_danmaku() {
    return owner->get_danmaku();
}

int Shot::get_time() {
    return time;
}

void Shot::set_effects(Array p_effects) {
    effects = ShotEffect::bitmask(p_effects);
}

void Shot::set_sprite(String p_key) {
    Danmaku* danmaku = get_danmaku();
    int id = danmaku->get_sprite_id(p_key);
    if (id < 0) {
        ERR_PRINT("Sprite key \"" + p_key + "\" does not exist!");
        return;
    }
    radius = danmaku->get_sprite(id)->collider_radius;
    sprite_id = id;
}

String Shot::get_sprite() {
    return owner->get_danmaku()->get_sprite(sprite_id)->key;
}

void Shot::set_velocity(Vector2 p_velocity) {
    speed = p_velocity.length();
    direction = p_velocity / speed;
}

Vector2 Shot::get_velocity() {
    return direction * speed;
}

void Shot::set_rotation(float p_rotation) {
    direction = Vector2(cos(p_rotation), sin(p_rotation));
}

float Shot::get_rotation() {
    return direction.angle();
}

void Shot::_register_methods() {
    register_property<Shot, float>("speed", &Shot::speed, 0);
    register_property<Shot, Vector2>("direction", &Shot::direction, Vector2(0, 1));
    register_property<Shot, Vector2>("position", &Shot::position, Vector2(0, 0));
    register_property<Shot, float>("radius", &Shot::radius, 0);
    register_property<Shot, String>("sprite", &Shot::set_sprite, &Shot::get_sprite, "");
    register_property<Shot, Vector2>("velocity", &Shot::set_velocity, &Shot::get_velocity, Vector2(0, 0));
    register_property<Shot, float>("rotation", &Shot::set_rotation, &Shot::get_rotation, 0);
    register_property<Shot, int>("time", nullptr, &Shot::get_time, 0);

    ClassDB::bind_method(D_METHOD("set_effects"), &Shot::set_effects);
    ClassDB::bind_method(D_METHOD("get_pattern"), &Shot::get_pattern);
    ClassDB::bind_method(D_METHOD("get_danmaku"), &Shot::get_danmaku);
}

void Shot::_init() {
    flags = 0;
    effects = 0;
    owner = nullptr;
    local_id = 0;
    global_id = 0;
    time = 0;
    global_position = Vector2(0, 0);
    position = Vector2(0, 0);
    direction = Vector2(0, 1);
    speed = 0;
    radius = 0;
    sprite_id = 0;
}