#include "danmaku.hpp"

using namespace godot;

void Danmaku::_register_methods()
{
    register_property<Danmaku, int>("max_shots", &Danmaku::max_shots, 2048);

    register_method("is_danmaku", &Danmaku::is_danmaku);
    register_method("_enter_tree", &Danmaku::_enter_tree);
    register_method("_exit_tree", &Danmaku::_exit_tree);
}

// Annoying workaround for there not being a good way to check if an object is a custom type. Hopefully I can fix this in Godot 4.0.
bool Danmaku::is_danmaku()
{
    return true;
}

Danmaku::Danmaku()
{
    _free_count = _max_shots = 0;
    _shots = nullptr;
    _free_ids = nullptr;
    max_shots = 2048;
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


void Danmaku::get_free_ids(int* buf, int count)
{
    for (int i = 0; i != count; ++i) {
        buf[i] = _free_ids[0];
        _free_ids[0] = _free_ids[--_free_count];
    }
}