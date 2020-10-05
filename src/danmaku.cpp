#include "danmaku.hpp"

using namespace godot;

void Danmaku::_register_methods()
{
    register_property<Danmaku, int>("max_shots", &Danmaku::max_shots, 2048);
    register_property<Danmaku, Rect2>("region", &Danmaku::region, Rect2(0, 0, 384, 448));
    register_property<Danmaku, int>("tolerance", &Danmaku::tolerance, 64);
    register_property<Danmaku, Array>("sprites", &Danmaku::sprites, Array());

    register_property<Danmaku, bool>("clear_enabled", &Danmaku::clear_enabled, false);
    register_property<Danmaku, Vector2>("clear_origin", &Danmaku::clear_origin, Vector2(0, 0));
    register_property<Danmaku, float>("clear_radius", &Danmaku::clear_radius, 0);

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
    _free_shots = nullptr;
    _hitbox = nullptr;

    clear_radius = 0;
    clear_origin = Vector2(0, 0);
    clear_enabled = false;
    max_shots = 2048;
    region = Rect2(0, 0, 384, 448);
    tolerance = 64;
    sprites = Array();
}

Danmaku::~Danmaku()
{
    
}

void Danmaku::_enter_tree()
{
    _free_count = _max_shots = max_shots;

    _free_shots = new Shot*[_max_shots];
    for (int i = 0; i != _max_shots; ++i) {
        _free_shots[i] = Shot::_new();
    }

    _sprite_count = sprites.size();
    _sprites = new ShotSprite*[_sprite_count];
    for (int i = 0; i != _sprite_count; ++i) {
        _sprites[i] = Object::cast_to<ShotSprite>(sprites[i]);
        _sprites[i]->reference();
    }
}

void Danmaku::_exit_tree()
{
    for (int i = 0; i != _max_shots; ++i) {
        _free_shots[i]->free();
    }
    delete[] _free_shots;

    for (int i = 0; i != _sprite_count; ++i) {
        if (_sprites[i]->unreference())
            _sprites[i]->free();
    }
    delete[] _sprites;
}


void Danmaku::capture(Shot** buf, int count)
{
    for (int i = 0; i != count; ++i) {
        buf[i] = _free_shots[0];
        _free_shots[0] = _free_shots[--_free_count];
    }
    _active_count += count;
}

void Danmaku::release(Shot** buf, int count)
{
    for (int i = 0; i != count; ++i) {
        _free_shots[_free_count++] = buf[i];
    }
    _active_count -= count;
}


int Danmaku::get_sprite_id(const String& key)
{
    for (int i = 0; i != _sprite_count; ++i) {
        if (_sprites[i]->key == key)
            return i;
    }
    return -1;
}


bool Danmaku::should_clear(Vector2 position)
{
    if (clear_enabled) {
        return position.distance_to(clear_origin) <= clear_radius;
    }
    return false;
}