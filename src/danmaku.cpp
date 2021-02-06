#include "danmaku.hpp"
#include "pattern.hpp"

#include <algorithm>

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
        sprites.push_back(Object::cast_to<ShotSprite>(registered_sprites[i]));
    }
}

void Danmaku::_exit_tree() {
    for (int i = 0; i != max_shots; ++i) {
        free_shots[i]->free();
    }
}

void Danmaku::add_pattern(Pattern* p_pattern) {
    patterns.push_back(p_pattern);
}

void Danmaku::remove_pattern(Pattern* p_pattern) {
    patterns.erase(std::remove(patterns.begin(), patterns.end(), p_pattern), patterns.end());
}

void Danmaku::add_hitbox(Hitbox* p_hitbox) {
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
    return max_shots - (int)free_shots.size();
}

int Danmaku::get_pattern_count() {
    return (int)patterns.size();
}

Shot* Danmaku::capture() {
    Shot* shot = free_shots.back();
    free_shots.pop_back();
    return shot;
}

void Danmaku::release(Shot* p_shot) {
    free_shots.push_back(p_shot);
}

Ref<ShotSprite> Danmaku::get_sprite(int p_id) {
    return sprites[p_id];
}

int Danmaku::get_sprite_id(const String& p_key) {
    for (int i = 0; i != sprites.size(); ++i) {
        if (sprites[i]->key == p_key)
            return i;
    }
    return 0; // Default to first sprite
}

Rect2 Danmaku::get_region() {
    return region.grow(tolerance);
}

void Danmaku::clear_circle(Vector2 p_origin, float p_radius) {
    for (Pattern* pattern : patterns) {
        pattern->clear([=](Shot* shot) {
            return (shot->global_position - p_origin).length() <= p_radius;
        });
    }
}

void Danmaku::clear_rect(Rect2 p_rect) {
    for (Pattern* pattern : patterns) {
        pattern->clear([=](Shot* shot) {
            return p_rect.has_point(shot->global_position);
        });
    }
}

void Danmaku::_register_methods() {
    register_property<Danmaku, int>("max_shots", &Danmaku::max_shots, 2048);
    register_property<Danmaku, Rect2>("region", &Danmaku::region, Rect2(0, 0, 384, 448));
    register_property<Danmaku, int>("tolerance", &Danmaku::tolerance, 64);
    register_property<Danmaku, Array>("sprites", &Danmaku::registered_sprites, Array());

    register_method("is_danmaku", &Danmaku::is_danmaku);
    register_method("_enter_tree", &Danmaku::_enter_tree);
    register_method("_exit_tree", &Danmaku::_exit_tree);

    register_method("get_free_shot_count", &Danmaku::get_free_shot_count);
    register_method("get_active_shot_count", &Danmaku::get_active_shot_count);
    register_method("get_pattern_count", &Danmaku::get_pattern_count);

    register_method("clear_circle", &Danmaku::clear_circle);
    register_method("clear_rect", &Danmaku::clear_rect);
}

void Danmaku::_init() {
    max_shots = 0;
    hitbox = nullptr;
    max_shots = 2048;
    region = Rect2(0, 0, 384, 448);
    tolerance = 64;
    registered_sprites = Array();
}