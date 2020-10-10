#include "shot.hpp"

#include "danmaku.hpp"
#include "pattern.hpp"

#include <Math.hpp>

using namespace godot;

void Shot::_register_methods()
{
    register_property<Shot, float>("speed", &Shot::speed, 0);
    register_property<Shot, Vector2>("direction", &Shot::direction, Vector2(0, 1));
    register_property<Shot, Vector2>("position", &Shot::position, Vector2(0, 0));
    register_property<Shot, float>("radius", &Shot::radius, 0);

    register_property<Shot, String>("sprite", &Shot::set_sprite, &Shot::get_sprite, "");
    register_property<Shot, Vector2>("velocity", &Shot::set_velocity, &Shot::get_velocity, Vector2(0, 0));
    register_property<Shot, float>("rotation", &Shot::set_rotation, &Shot::get_rotation, 0);

    register_method("get_pattern", &Shot::get_pattern);
    register_method("get_time", &Shot::get_time);
}

void Shot::_init()
{
    reset();
}

void Shot::reset()
{
    owner = nullptr;
    time = 0;
    active = false;
    global_position = Vector2(0, 0);
    position = Vector2(0, 0);
    direction = Vector2(0, 1);
    speed = 0;
    radius = 0;
    sprite_id = 0;
}

void Shot::set_sprite(String key)
{
    Danmaku* danmaku = owner->get_danmaku();
    int id = danmaku->get_sprite_id(key);
    if (id < 0) {
        Godot::print_error("Sprite key \"" + key + "\" does not exist!", "set_sprite", __FILE__, __LINE__);
        return;
    }
    radius = danmaku->get_sprite(id)->collider_radius;
    sprite_id = id;
}

String Shot::get_sprite()
{
    return owner->get_danmaku()->get_sprite(sprite_id)->key;
}

void Shot::set_velocity(Vector2 velocity)
{
    speed = velocity.length();
    direction = velocity / speed;
}

Vector2 Shot::get_velocity()
{
    return direction * speed;
}

void Shot::set_rotation(float rotation)
{
    direction = Vector2(cos(rotation), sin(rotation));
}

float Shot::get_rotation()
{
    return atan2(direction.y, direction.x);
}