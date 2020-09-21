#include "pattern.hpp"

#include <Math.hpp>

using namespace godot;

void Pattern::_register_methods()
{
    register_method("_enter_tree", &Pattern::_enter_tree);
    register_method("_physics_process", &Pattern::_physics_process);
    register_method("_draw", &Pattern::_draw);

    register_method("create_circle", &Pattern::create_circle);
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
}

void Pattern::_physics_process(float delta)
{
    for (int i = 0; i != _shot_count; ++i) {
        Shot* shot = _danmaku->get_shot(_shot_ids[i]);
        shot->position += shot->direction * shot->speed;
    }
    update();
}

void Pattern::_draw()
{
    for (int i = 0; i != _shot_count; ++i) {
        Shot* shot = _danmaku->get_shot(_shot_ids[i]);
        draw_circle(shot->position, 10, Color(1, 1, 1));
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
    int* buf = &_shot_ids[_shot_count];
    _danmaku->get_free_ids(buf, count);
    _shot_count += count;
    return buf;
}


void Pattern::create_circle(int count, float speed)
{
    if (_danmaku == nullptr) return;

    int* buf = buffer(count);

    for (int i = 0; i != count; ++i) {
        float angle = i * (Math_TAU / (float)count);

        Shot* shot = _danmaku->get_shot(buf[i]);
        shot->direction = Vector2(cos(angle), sin(angle));
        shot->position = Vector2(0, 0);
        shot->speed = speed;
    }
}