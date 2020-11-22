#include "danmaku.hpp"

using namespace godot;

// Annoying workaround for there not being a good way to check if an object is a custom type. Hopefully I can fix this in Godot 4.0.
bool Danmaku::is_danmaku() {
    return true;
}

void Danmaku::_enter_tree() {
    for (int i = 0; i != max_shots; ++i) {
        Shot* shot = Shot::_new();
        shot->global_id = i;
        free_shots.push_back(shot);
    }

    for (int i = 0; i != registered_sprites.size(); ++i) {
        ShotSprite* sprite = Object::cast_to<ShotSprite>(registered_sprites[i]);
        sprite->reference();
        sprites.push_back(sprite);
    }
}

void Danmaku::_exit_tree() {
    for (int i = 0; i != max_shots; ++i) {
        free_shots[i]->free();
    }

    for (ShotSprite* sprite : sprites) {
        if (sprite->unreference())
            sprite->free();
    }
}

void Danmaku::count_pattern() {
    active_patterns++;
}

void Danmaku::decount_pattern() {
    active_patterns--;
}

void Danmaku::register_hitbox(Hitbox* p_hitbox) {
    hitbox = p_hitbox;
}

void Danmaku::remove_hitbox() {
    hitbox = nullptr;
}

Hitbox* Danmaku::get_hitbox() {
    return hitbox;
}

int Danmaku::get_free_shot_count() {
    return (int)free_shots.size();
}

int Danmaku::get_active_shot_count() {
    return active_shots;
}

int Danmaku::get_pattern_count() {
    return active_patterns;
}

Shot* Danmaku::capture() {
    active_shots++;
    Shot* shot = free_shots.back();
    free_shots.pop_back();
    return shot;
}

void Danmaku::release(Shot* p_shot) {
    active_shots--;
    free_shots.push_back(p_shot);
}

ShotSprite* Danmaku::get_sprite(int p_id) {
    return sprites[p_id];
}

int Danmaku::get_sprite_id(const String& p_key) {
    for (int i = 0; i != sprites.size(); ++i) {
        if (sprites[i]->key == p_key)
            return i;
    }
    return -1;
}

Rect2 Danmaku::get_region() {
    return region.grow(tolerance);
}

bool Danmaku::should_clear(Vector2 p_position) {
    if (clear_enabled) {
        return p_position.distance_to(clear_origin) <= clear_radius;
    }
    return false;
}

void Danmaku::_register_methods() {
    register_property<Danmaku, int>("max_shots", &Danmaku::max_shots, 2048);
    register_property<Danmaku, Rect2>("region", &Danmaku::region, Rect2(0, 0, 384, 448));
    register_property<Danmaku, int>("tolerance", &Danmaku::tolerance, 64);
    register_property<Danmaku, Array>("sprites", &Danmaku::registered_sprites, Array());

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

void Danmaku::_init() {
    active_shots = 0;
    active_patterns = 0;
    max_shots = 0;
    hitbox = nullptr;
    clear_radius = 0;
    clear_origin = Vector2(0, 0);
    clear_enabled = false;
    max_shots = 2048;
    region = Rect2(0, 0, 384, 448);
    tolerance = 64;
    registered_sprites = Array();
}