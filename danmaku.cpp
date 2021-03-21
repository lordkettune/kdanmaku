#include "danmaku.h"
#include "pattern.h"

#define MAX_SHOT_SPRITES 32

void Danmaku::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_ENTER_TREE: {
            for (int i = 0; i != max_shots; ++i) {
                Shot* shot = memnew(Shot);
                free_shots.push_back(shot);
            }
        } break;

        case NOTIFICATION_EXIT_TREE: {
            for (int i = 0; i != max_shots; ++i) {
                memdelete(free_shots[i]);
            }
        } break;
    }
}

void Danmaku::add_pattern(Pattern* p_pattern) {
    patterns.push_back(p_pattern);
}

void Danmaku::remove_pattern(Pattern* p_pattern) {
    patterns.erase(p_pattern);
}

void Danmaku::add_hitbox(Hitbox* p_hitbox) {
    hitbox = p_hitbox;
}

void Danmaku::remove_hitbox() {
    hitbox = NULL;
}

Hitbox* Danmaku::get_hitbox() const {
    return hitbox;
}

Shot* Danmaku::capture() {
    Shot* shot = free_shots[free_shots.size() - 1];
    free_shots.remove(free_shots.size() - 1);
    return shot;
}

void Danmaku::release(Shot* p_shot) {
    free_shots.push_back(p_shot);
}

Ref<ShotSprite> Danmaku::get_sprite(const String& p_key) const {
    for (int i = 0; i != sprites.size(); ++i) {
        if (sprites[i]->get_key() == p_key)
            return sprites[i];
    }
    ERR_FAIL_V_MSG(Ref<ShotSprite>(), "Failed to find shot sprite");
}

void Danmaku::clear_circle(Vector2 p_origin, float p_radius) {
    for (int i = 0; i != patterns.size(); ++i) {
        Transform2D transform = patterns[i]->get_global_transform();
        patterns[i]->clear([=](Shot* shot) {
            return (transform.xform(shot->get_position()) - p_origin).length() <= p_radius;
        });
    }
}

void Danmaku::clear_rect(Rect2 p_rect) {
    for (int i = 0; i != patterns.size(); ++i) {
        Transform2D transform = patterns[i]->get_global_transform();
        patterns[i]->clear([=](Shot* shot) {
            return p_rect.has_point(transform.xform(shot->get_position()));
        });
    }
}

int Danmaku::get_free_shot_count() const {
    return free_shots.size();
}

int Danmaku::get_active_shot_count() const {
    return max_shots - free_shots.size();
}

int Danmaku::get_pattern_count() const {
    return patterns.size();
}

void Danmaku::set_region(const Rect2& p_region) {
    region = p_region;
}

Rect2 Danmaku::get_region() const {
    return region;
}

void Danmaku::set_tolerance(float p_tolerance) {
    tolerance = p_tolerance;
}

float Danmaku::get_tolerance() const {
    return tolerance;
}

void Danmaku::set_max_shots(int p_max_shots) {
    max_shots = p_max_shots;
}

int Danmaku::get_max_shots() const {
    return max_shots;
}

void Danmaku::set_shot_sprite_count(int p_count) {
    ERR_FAIL_COND(p_count < 1);
    sprites.resize(p_count);
    _change_notify();
}

int Danmaku::get_shot_sprite_count() const {
    return sprites.size();
}

void Danmaku::set_shot_sprite(int p_index, const Ref<ShotSprite>& p_sprite) {
    ERR_FAIL_INDEX(p_index, sprites.size());
    sprites.write[p_index] = p_sprite;
}

Ref<ShotSprite> Danmaku::get_shot_sprite(int p_index) const {
    ERR_FAIL_INDEX_V(p_index, sprites.size(), Ref<ShotSprite>());
    return sprites[p_index];
}

void Danmaku::_validate_property(PropertyInfo& property) const {
	if (property.name.begins_with("shot_sprite_")) {
		int index = property.name.get_slicec('_', 2).to_int() - 1;
		if (index >= sprites.size()) {
			property.usage = 0;
			return;
		}
	}
}

void Danmaku::_bind_methods() {
    ClassDB::bind_method(D_METHOD("clear_circle"), &Danmaku::clear_circle);
    ClassDB::bind_method(D_METHOD("clear_rect"), &Danmaku::clear_rect);
    
    ClassDB::bind_method(D_METHOD("get_free_shot_count"), &Danmaku::get_free_shot_count);
    ClassDB::bind_method(D_METHOD("get_active_shot_count"), &Danmaku::get_active_shot_count);
    ClassDB::bind_method(D_METHOD("get_pattern_count"), &Danmaku::get_pattern_count);

    ClassDB::bind_method(D_METHOD("set_max_shots", "max_shots"), &Danmaku::set_max_shots);
    ClassDB::bind_method(D_METHOD("set_region", "region"), &Danmaku::set_region);
    ClassDB::bind_method(D_METHOD("set_tolerance", "tolerance"), &Danmaku::set_tolerance);

    ClassDB::bind_method(D_METHOD("get_max_shots"), &Danmaku::get_max_shots);
    ClassDB::bind_method(D_METHOD("get_region"), &Danmaku::get_region);
    ClassDB::bind_method(D_METHOD("get_tolerance"), &Danmaku::get_tolerance);

    ClassDB::bind_method(D_METHOD("set_shot_sprite_count", "count"), &Danmaku::set_shot_sprite_count);
    ClassDB::bind_method(D_METHOD("get_shot_sprite_count"), &Danmaku::get_shot_sprite_count);
    ClassDB::bind_method(D_METHOD("set_shot_sprite", "index", "sprite"), &Danmaku::set_shot_sprite);
    ClassDB::bind_method(D_METHOD("get_shot_sprite", "index"), &Danmaku::get_shot_sprite);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "max_shots"), "set_max_shots", "get_max_shots");
    ADD_PROPERTY(PropertyInfo(Variant::RECT2, "region"), "set_region", "get_region");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "tolerance"), "set_tolerance", "get_tolerance");

    ADD_GROUP("Sprites", "shot_");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "shot_sprites", PROPERTY_HINT_EXP_RANGE, "0," + itos(MAX_SHOT_SPRITES) + ",1"), "set_shot_sprite_count", "get_shot_sprite_count");
    for (int i = 0; i != MAX_SHOT_SPRITES; ++i) {
        ADD_PROPERTYI(PropertyInfo(Variant::OBJECT, "shot_sprite_" + itos(i + 1), PROPERTY_HINT_RESOURCE_TYPE, "ShotSprite"), "set_shot_sprite", "get_shot_sprite", i);
    }
}

Danmaku::Danmaku() {
    hitbox = NULL;
    max_shots = 2048;
    region = Rect2(0, 0, 384, 448);
    tolerance = 64;
    set_shot_sprite_count(1);
}