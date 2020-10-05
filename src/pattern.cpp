#include "pattern.hpp"

#include <Math.hpp>

#include "hitbox.hpp"

using namespace godot;

void Pattern::_register_methods()
{
    register_method("_enter_tree", &Pattern::_enter_tree);
    register_method("_exit_tree", &Pattern::_exit_tree);
    register_method("_physics_process", &Pattern::_physics_process);
    register_method("_draw", &Pattern::_draw);

    register_method("get_shot", &Pattern::get_shot);

    register_method("fire", &Pattern::fire);
    register_method("fire_layered", &Pattern::fire_layered);
    register_method("fire_circle", &Pattern::fire_circle);
    register_method("fire_layered_circle", &Pattern::fire_layered_circle);
    register_method("fire_fan", &Pattern::fire_fan);
    register_method("fire_layered_fan", &Pattern::fire_layered_fan);
}

Pattern::Pattern()
{
    _danmaku = nullptr;
    _shots = nullptr;
    _shots_size = 0;
    _active_count = 0;
}

Pattern::~Pattern()
{
    if (_shots != nullptr) {
        api->godot_free(_shots);
    }
}

void Pattern::_enter_tree()
{
    Node* parent = get_parent();
    while ((bool)parent->call("is_danmaku") != true) {
        parent = parent->get_parent();
        if (parent == nullptr) {
            Godot::print_error("Pattern is not a descendent of a Danmaku node!", "_enter_tree", __FILE__, __LINE__);
            return;
        }
    }
    _danmaku = Object::cast_to<Danmaku>(parent);
    _danmaku->count_pattern();
    set_z_index(_danmaku->get_z_index());
}

void Pattern::_exit_tree()
{
    // Release all shot IDs when exiting tree
    if (_active_count != 0) {
        _danmaku->release(_shots, _active_count);
        _active_count = 0;
    }
    _danmaku->decount_pattern();
}

void Pattern::_physics_process(float delta)
{
    if (_danmaku == nullptr) return;

    Transform2D transform = get_global_transform();
    Rect2 region = _danmaku->get_region().grow(_danmaku->get_tolerance());

    Hitbox* hitbox = _danmaku->get_hitbox();
    Vector2 hitbox_pos = Vector2(0, 0);
    if (hitbox != nullptr) {
        hitbox_pos = hitbox->get_global_transform().get_origin();
    }

    int release_count = 0;

    for (int i = 0; i != _active_count; ++i) {
        Shot* shot = _shots[i];
        if (!shot->active)
            continue;

        shot->position += shot->direction * shot->speed;
        shot->global_position = transform.xform(shot->position);

        if (hitbox != nullptr) {
            if (shot->global_position.distance_to(hitbox_pos) <= hitbox->radius + shot->radius) {
                hitbox->hit();
            }
        }

        if (!region.has_point(shot->global_position) || _danmaku->should_clear(shot->global_position)) {
            shot->active = false;
            ++release_count;
        }
    }

    // Shots left danmaku region, release their IDs
    if (release_count != 0) {
        for (int j = 0; j != release_count; ++j) {
            int i = 0;
            while (i != _active_count) {
                if (!_shots[i]->active) {
                    Shot* temp = _shots[--_active_count];
                    _shots[_active_count] = _shots[i];
                    _shots[i] = temp;
                    break;
                }
                ++i;
            }
        }
        _danmaku->release(_shots + _active_count, release_count);
    }

    update();
}

void Pattern::_draw()
{
    for (int i = 0; i != _active_count; ++i) {
        ShotSprite* sprite = _danmaku->get_sprite(_shots[i]->sprite_id);
        sprite->draw_to(this, _shots[i]->position);
    }
}


Shot** Pattern::buffer(int count)
{
    if (_shots == nullptr) {
        _shots_size = count;
        _shots = (Shot**)api->godot_alloc(_shots_size * sizeof(Shot*));
    } else {
        while (_shots_size < _active_count + count) {
            _shots_size *= 2;
        }
        _shots = (Shot**)api->godot_realloc(_shots, _shots_size * sizeof(Shot*));
    }
    Shot** buf = _shots + _active_count;
    _danmaku->capture(buf, count);
    _active_count += count;
    return buf;
}


bool Pattern::prepare(const String& sprite, int& sprite_id, float& radius, float& angle, int count, bool aim, Shot**& buf)
{
    if (_danmaku == nullptr) {
        Godot::print_error("Pattern is not a descendent of a Danmaku node!", "prepare", __FILE__, __LINE__);
        return false;
    }

    sprite_id = _danmaku->get_sprite_id(sprite);
    if (sprite_id < 0) {
        Godot::print_error("Sprite key \"" + sprite + "\" does not exist!", "prepare", __FILE__, __LINE__);
        return false;
    }

    radius = _danmaku->get_sprite(sprite_id)->collider_radius;

    if (aim) {
        Hitbox* hitbox = _danmaku->get_hitbox();
        if (hitbox == nullptr) {
            Godot::print_error("No Hitbox to aim at, make sure Hitbox is a descendent of Danmaku!", "prepare", __FILE__, __LINE__);
            return false;
        }
        angle += get_global_position().angle_to_point(hitbox->get_global_position());
    }

    buf = buffer(count);
    return true;
}

void Pattern::fire(String sprite, float speed, float angle, bool aim)
{
    int sprite_id;
    float radius;
    Shot** buf;
    if (prepare(sprite, sprite_id, radius, angle, 1, aim, buf)) {
        Shot* shot = buf[0];
        shot->direction = Vector2(-cos(angle), -sin(angle));
        shot->position = Vector2(0, 0);
        shot->speed = speed;
        shot->sprite_id = sprite_id;
        shot->radius = radius;
        shot->active = true;
    }
}

void Pattern::fire_layered(String sprite, int layers, float min_speed, float max_speed, float angle, bool aim)
{
    int sprite_id;
    float radius;
    Shot** buf;
    if (prepare(sprite, sprite_id, radius, angle, layers, aim, buf)) {
        float cx = -cos(angle);
        float sy = -sin(angle);
        float step = (max_speed - min_speed) / layers;
        for (int i = 0; i != layers; ++i) {
            Shot* shot = buf[i];
            shot->direction = Vector2(cx, sy);
            shot->position = Vector2(0, 0);
            shot->speed = min_speed + step * i;
            shot->sprite_id = sprite_id;
            shot->radius = radius;
            shot->active = true;
        }
    }
}

void Pattern::fire_circle(String sprite, int count, float speed, float angle, bool aim)
{
    int sprite_id;
    float radius;
    Shot** buf;
    if (prepare(sprite, sprite_id, radius, angle, count, aim, buf)) {
        for (int i = 0; i != count; ++i) {
            float shot_angle = angle + i * (Math_TAU / (float)count);

            Shot* shot = buf[i];
            shot->direction = Vector2(-cos(shot_angle), -sin(shot_angle));
            shot->position = Vector2(0, 0);
            shot->speed = speed;
            shot->sprite_id = sprite_id;
            shot->radius = radius;
            shot->active = true;
        }
    }
}

void Pattern::fire_layered_circle(String sprite, int count, int layers, float min_speed, float max_speed, float angle, bool aim)
{
    float step = (max_speed - min_speed) / layers;
    for (int i = 0; i != layers; ++i) {
        fire_circle(sprite, count, min_speed + step * i, angle, aim);
    }
}

void Pattern::fire_fan(String sprite, int count, float speed, float angle, float theta, bool aim)
{
    int sprite_id;
    float radius;
    Shot** buf;
    if (prepare(sprite, sprite_id, radius, angle, count, aim, buf)) {
        float base = angle - theta * 0.5f;
        float step = theta / (count - 1);
        for (int i = 0; i != count; ++i) {
            float shot_angle = base + i * step;

            Shot* shot = buf[i];
            shot->direction = Vector2(-cos(shot_angle), -sin(shot_angle));
            shot->position = Vector2(0, 0);
            shot->speed = speed;
            shot->sprite_id = sprite_id;
            shot->radius = radius;
            shot->active = true;
        }
    }
}

void Pattern::fire_layered_fan(String sprite, int count, int layers, float min_speed, float max_speed, float angle, float theta, bool aim)
{
    float step = (max_speed - min_speed) / layers;
    for (int i = 0; i != layers; ++i) {
        fire_fan(sprite, count, min_speed + step * i, angle, theta, aim);
    }
}