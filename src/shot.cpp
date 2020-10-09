#include "shot.hpp"

#include "pattern.hpp"

using namespace godot;

void Shot::_register_methods()
{
    register_property<Shot, float>("speed", &Shot::speed, 0);
    register_property<Shot, Vector2>("direction", &Shot::direction, Vector2(0, 1));
    register_property<Shot, Vector2>("position", &Shot::position, Vector2(0, 0));
    register_property<Shot, float>("radius", &Shot::radius, 0);

    register_method("get_pattern", &Shot::get_pattern);
}

void Shot::_init()
{
    owner = nullptr;
    active = false;
    global_position = Vector2(0, 0);
    position = Vector2(0, 0);
    direction = Vector2(0, 1);
    speed = 0;
    radius = 0;
    sprite_id = 0;
}