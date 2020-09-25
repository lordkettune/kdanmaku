#include "danmaku.hpp"

using namespace godot;

void Danmaku::_register_methods()
{
    register_property<Danmaku, int>("max_shots", &Danmaku::max_shots, 2048);
    register_property<Danmaku, Rect2>("region", &Danmaku::region, Rect2(0, 0, 384, 448));
    register_property<Danmaku, int>("tolerance", &Danmaku::tolerance, 64);

    register_method("is_danmaku", &Danmaku::is_danmaku);
    register_method("_enter_tree", &Danmaku::_enter_tree);
    register_method("_exit_tree", &Danmaku::_exit_tree);

    register_method("get_free_shot_count", &Danmaku::get_free_shot_count);
    register_method("get_active_shot_count", &Danmaku::get_active_shot_count);
    register_method("get_pattern_count", &Danmaku::get_pattern_count);
}

// Annoying workaround for there not being a good way to check if an object is a custom type. Hopefully I can fix this in Godot 4.0.
bool Danmaku::is_danmaku()
{
    return true;
}

Danmaku::Danmaku()
{
    _free_count = 0;
    _active_count = 0;
    _pattern_count = 0;
    _max_shots = 0;
    _shots = nullptr;
    _free_ids = nullptr;

    max_shots = 2048;
    region = Rect2(0, 0, 384, 448);
    tolerance = 64;
}

Danmaku::~Danmaku()
{
    _exit_tree();
}

void Danmaku::_enter_tree()
{
    _free_count = _max_shots = max_shots;
    _shots = (Shot*)api->godot_alloc(_max_shots * sizeof(Shot));
    _free_ids = (int*)api->godot_alloc(_max_shots * sizeof(int));

    for (int i = 0; i != _max_shots; ++i) {
        _free_ids[i] = i;
    }
}

void Danmaku::_exit_tree()
{
    if (_shots != nullptr) api->godot_free(_shots);
    if (_free_ids != nullptr) api->godot_free(_free_ids);
}


void Danmaku::capture_ids(int* buf, int count)
{
    for (int i = 0; i != count; ++i) {
        buf[i] = _free_ids[0];
        _free_ids[0] = _free_ids[--_free_count];
    }
    _active_count += count;
}

void Danmaku::release_ids(int* buf, int count)
{
    for (int i = 0; i != count; ++i) {
        _free_ids[_free_count++] = buf[i];
    }
    _active_count -= count;
}