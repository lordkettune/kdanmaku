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
    _shot_ids = nullptr;
    _shot_ids_size = 0;
    _shot_count = 0;
}

Pattern::~Pattern()
{
    if (_shot_ids != nullptr) {
        api->godot_free(_shot_ids);
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
    if (_shot_count != 0) {
        _danmaku->release_ids(_shot_ids, _shot_count);
        _shot_count = 0;
    }
    _danmaku->decount_pattern();
}

void Pattern::_physics_process(float delta)
{
    if (_danmaku == nullptr) return;

    Transform2D transform = get_relative_transform_to_parent(_danmaku);
    Rect2 region = _danmaku->get_region().grow(_danmaku->get_tolerance());

    Hitbox* hitbox = _danmaku->get_hitbox();
    Vector2 hitbox_pos = Vector2(0, 0);
    if (hitbox != nullptr) {
        hitbox_pos = hitbox->get_relative_transform_to_parent(_danmaku).get_origin();
    }

    int release_count = 0;

    for (int i = 0; i != _shot_count; ++i) {
        Shot* shot = _danmaku->get_shot(_shot_ids[i]);
        if (!shot->active)
            continue;

        shot->position += shot->direction * shot->speed;
        shot->relative_position = transform.xform(shot->position);

        if (hitbox != nullptr) {
            if (shot->relative_position.distance_to(hitbox_pos) <= hitbox->radius + shot->radius) {
                hitbox->hit();
            }
        }

        if (!region.has_point(shot->relative_position) || _danmaku->should_clear(shot->relative_position)) {
            shot->active = false;
            ++release_count;
        }
    }

    // Shots left danmaku region, release their IDs
    if (release_count != 0) {
        for (int j = 0; j != release_count; ++j) {
            int i = 0;
            while (i != _shot_count) {
                Shot* shot = _danmaku->get_shot(_shot_ids[i]);
                if (!shot->active) {
                    int temp = _shot_ids[--_shot_count];
                    _shot_ids[_shot_count] = _shot_ids[i];
                    _shot_ids[i] = temp;
                    break;
                }
                ++i;
            }
        }
        _danmaku->release_ids(_shot_ids + _shot_count, release_count);
    }

    update();
}

void Pattern::_draw()
{
    for (int i = 0; i != _shot_count; ++i) {
        Shot* shot = _danmaku->get_shot(_shot_ids[i]);
        ShotSprite* sprite = _danmaku->get_sprite(shot->sprite_id);

        if (shot->active)
            sprite->draw_to(this, shot->position);
    }
}


int* Pattern::buffer(int count)
{
    if (_shot_ids == nullptr) {
        _shot_ids_size = count;
        _shot_ids = (int*)api->godot_alloc(_shot_ids_size * sizeof(int));
    } else {
        while (_shot_ids_size < _shot_count + count) {
            _shot_ids_size *= 2;
        }
        _shot_ids = (int*)api->godot_realloc(_shot_ids, _shot_ids_size * sizeof(int));
    }
    int* buf = _shot_ids + _shot_count;
    _danmaku->capture_ids(buf, count);
    _shot_count += count;
    return buf;
}


bool Pattern::prepare(const String& sprite, int& sprite_id, float& radius, float& angle, int count, bool aim, int*& buf)
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
    int* buf;
    if (prepare(sprite, sprite_id, radius, angle, 1, aim, buf)) {
        Shot* shot = _danmaku->get_shot(buf[0]);
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
    int* buf;
    if (prepare(sprite, sprite_id, radius, angle, layers, aim, buf)) {
        float cx = -cos(angle);
        float sy = -sin(angle);
        float step = (max_speed - min_speed) / layers;
        for (int i = 0; i != layers; ++i) {
            Shot* shot = _danmaku->get_shot(buf[i]);
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
    int* buf;
    if (prepare(sprite, sprite_id, radius, angle, count, aim, buf)) {
        for (int i = 0; i != count; ++i) {
            float shot_angle = angle + i * (Math_TAU / (float)count);

            Shot* shot = _danmaku->get_shot(buf[i]);
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
    int* buf;
    if (prepare(sprite, sprite_id, radius, angle, count, aim, buf)) {
        float base = angle - theta * 0.5f;
        float step = theta / (count - 1);
        for (int i = 0; i != count; ++i) {
            float shot_angle = base + i * step;

            Shot* shot = _danmaku->get_shot(buf[i]);
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