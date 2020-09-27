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

    register_method("fire_circle", &Pattern::fire_circle);
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
            api->godot_print_error("Pattern is not a child of Danmaku!", "_enter_tree", __FILE__, __LINE__);
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

        if (!region.has_point(shot->relative_position)) {
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


void Pattern::fire_circle(String sprite, int count, float speed)
{
    if (_danmaku == nullptr) return;

    int sprite_id = _danmaku->get_sprite_id(sprite);
    float radius = _danmaku->get_sprite(sprite_id)->collider_radius;

    int* buf = buffer(count);

    for (int i = 0; i != count; ++i) {
        float angle = i * (Math_TAU / (float)count);

        Shot* shot = _danmaku->get_shot(buf[i]);
        shot->direction = Vector2(cos(angle), sin(angle));
        shot->position = Vector2(0, 0);
        shot->speed = speed;
        shot->active = true;
        shot->sprite_id = sprite_id;
        shot->radius = radius;
    }
}