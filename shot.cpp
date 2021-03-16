#include "shot.h"
#include "danmaku.h"
#include "pattern.h"

#include "core/math/math_funcs.h"

enum {
    PROP_SPEED
};

void Shot::set_register(Register p_reg, const Variant& p_value) {
    switch (p_reg) {
        case POSITION:  set_position(p_value);  break;
        case SPEED:     set_speed(p_value);     break;
        case DIRECTION: set_direction(p_value); break;
        case ROTATION:  set_rotation(p_value);  break;
        case VELOCITY:  set_velocity(p_value);  break;
        default: registers[p_reg >> 2] = p_value; break;
    }
}

Variant Shot::get_register(Register p_reg) const {
    switch (p_reg) {
        case POSITION:  return get_position();
        case SPEED:     return get_speed();
        case DIRECTION: return get_direction();
        case ROTATION:  return get_rotation();
        case VELOCITY:  return get_velocity();
        default: return registers[p_reg >> 2];
    }
}

void Shot::reset(Pattern* p_owner, int p_id) {
    id = p_id;
    owner = p_owner;
    flags = FLAG_ACTIVE;
    sprite = 0;
    global_position = Vector2(0, 0);
    position = Vector2(0, 0);
    direction = Vector2(0, 1);
    speed = 0;
    radius = 0;
}

Pattern* Shot::get_pattern() const {
    return owner;
}

Danmaku* Shot::get_danmaku() const {
    return owner->get_danmaku();
}

void Shot::set_global_position(const Vector2& p_global_position) {
    global_position = p_global_position;
}

Vector2 Shot::get_global_position() const {
    return global_position;
}

void Shot::set_sprite(const String& p_key) {
    Danmaku* danmaku = get_danmaku();
    int id = danmaku->get_sprite_id(p_key);
    radius = danmaku->get_sprite(id)->get_collider_radius();
    sprite = id;
}

String Shot::get_sprite() const {
    if (owner) {
        return owner->get_danmaku()->get_sprite(sprite)->get_key();
    } else {
        return "";
    }
}

void Shot::set_speed(float p_speed) {
    speed = p_speed;
}

float Shot::get_speed() const {
    return speed;
}

void Shot::set_direction(const Vector2& p_direction) {
    direction = p_direction;
}

Vector2 Shot::get_direction() const {
    return direction;
}

void Shot::set_position(const Vector2& p_position) {
    position = p_position;
}

Vector2 Shot::get_position() const {
    return position;
}

void Shot::set_radius(float p_radius) {
    radius = p_radius;
}

float Shot::get_radius() const {
    return radius;
}

void Shot::set_velocity(const Vector2& p_velocity) {
    speed = p_velocity.length();
    direction = p_velocity / speed;
}

Vector2 Shot::get_velocity() const {
    return direction * speed;
}

void Shot::set_rotation(float p_rotation) {
    direction = Vector2(Math::cos(p_rotation), Math::sin(p_rotation));
}

float Shot::get_rotation() const {
    return direction.angle();
}

void Shot::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_pattern"), &Shot::get_pattern);
    ClassDB::bind_method(D_METHOD("get_danmaku"), &Shot::get_danmaku);

    ClassDB::bind_method(D_METHOD("set_speed", "speed"), &Shot::set_speed);
    ClassDB::bind_method(D_METHOD("set_direction", "direction"), &Shot::set_direction);
    ClassDB::bind_method(D_METHOD("set_position", "position"), &Shot::set_position);
    ClassDB::bind_method(D_METHOD("set_velocity", "velocity"), &Shot::set_velocity);
    ClassDB::bind_method(D_METHOD("set_rotation", "rotation"), &Shot::set_rotation);
    ClassDB::bind_method(D_METHOD("set_radius", "radius"), &Shot::set_radius);
    ClassDB::bind_method(D_METHOD("set_sprite", "sprite"), &Shot::set_sprite);

    ClassDB::bind_method(D_METHOD("get_speed"), &Shot::get_speed);
    ClassDB::bind_method(D_METHOD("get_direction"), &Shot::get_direction);
    ClassDB::bind_method(D_METHOD("get_position"), &Shot::get_position);
    ClassDB::bind_method(D_METHOD("get_velocity"), &Shot::get_velocity);
    ClassDB::bind_method(D_METHOD("get_rotation"), &Shot::get_rotation);
    ClassDB::bind_method(D_METHOD("get_radius"), &Shot::get_radius);
    ClassDB::bind_method(D_METHOD("get_sprite"), &Shot::get_sprite);

    ADD_PROPERTY(PropertyInfo(Variant::REAL, "speed"), "set_speed", "get_speed");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "direction"), "set_direction", "get_direction");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "position"), "set_position", "get_position");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "velocity"), "set_velocity", "get_velocity");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "rotation"), "set_rotation", "get_rotation");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "radius"), "set_radius", "get_radius");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "sprite"), "set_sprite", "get_sprite");

    BIND_CONSTANT(REG0);
    BIND_CONSTANT(REG1);
    BIND_CONSTANT(REG2);
    BIND_CONSTANT(REG3);
    BIND_CONSTANT(REG4);
    BIND_CONSTANT(REG5);
    BIND_CONSTANT(REG6);
    BIND_CONSTANT(REG7);
    BIND_CONSTANT(POSITION);
    BIND_CONSTANT(SPEED);
    BIND_CONSTANT(DIRECTION);
    BIND_CONSTANT(ROTATION);
    BIND_CONSTANT(VELOCITY);
}

Shot::Shot() {
    id = 0;
    owner = NULL;
    flags = 0;
    sprite = 0;
    global_position = Vector2(0, 0);
    position = Vector2(0, 0);
    direction = Vector2(0, 1);
    speed = 0;
    radius = 0;
}