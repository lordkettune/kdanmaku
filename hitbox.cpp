#include "hitbox.h"

void Hitbox::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_ENTER_TREE: {
            Node* parent = this;
            while (parent) {
                danmaku = Object::cast_to<Danmaku>(parent);
                if (danmaku) {
                    danmaku->add_hitbox(this);
                    break;
                }
                parent = parent->get_parent();
            }
        } break;

        case NOTIFICATION_EXIT_TREE: {
            if (danmaku) {
                danmaku->remove_hitbox();
            }
        } break;
    }
}


void Hitbox::hit(Shot* p_shot) {
    if (invulnerable) {
        return;
    }
    colliding_shot = p_shot;
    emit_signal("hit");
}

void Hitbox::graze(Shot* p_shot) {
    if (invulnerable) {
        return;
    }
    grazing_shot = p_shot;
    emit_signal("graze");
}

Danmaku* Hitbox::get_danmaku() const {
    return danmaku;
}

void Hitbox::remove_from_danmaku() {
    danmaku->remove_hitbox();
    danmaku = nullptr;
}

Shot* Hitbox::get_colliding_shot() const {
    return colliding_shot;
}

Shot* Hitbox::get_grazing_shot() const {
    return grazing_shot;
}

void Hitbox::set_invulnerable(bool p_invulnerable) {
    invulnerable = p_invulnerable;
}

bool Hitbox::is_invulnerable() const {
    return invulnerable;
}

void Hitbox::set_collision_radius(float p_collision_radius) {
    collision_radius = p_collision_radius;
}

float Hitbox::get_collision_radius() const {
    return collision_radius;
}

void Hitbox::set_graze_radius(float p_graze_radius) {
    graze_radius = p_graze_radius;
}

float Hitbox::get_graze_radius() const {
    return graze_radius;
}

void Hitbox::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_danmaku"), &Hitbox::get_danmaku);
    ClassDB::bind_method(D_METHOD("get_colliding_shot"), &Hitbox::get_colliding_shot);
    ClassDB::bind_method(D_METHOD("get_grazing_shot"), &Hitbox::get_grazing_shot);

    ClassDB::bind_method(D_METHOD("set_invulnerable", "invulnerable"), &Hitbox::set_invulnerable);
    ClassDB::bind_method(D_METHOD("set_collision_radius", "collision_radius"), &Hitbox::set_collision_radius);
    ClassDB::bind_method(D_METHOD("set_graze_radius", "graze_radius"), &Hitbox::set_graze_radius);

    ClassDB::bind_method(D_METHOD("is_invulnerable"), &Hitbox::is_invulnerable);
    ClassDB::bind_method(D_METHOD("get_collision_radius"), &Hitbox::get_collision_radius);
    ClassDB::bind_method(D_METHOD("get_graze_radius"), &Hitbox::get_graze_radius);

    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "invulnerable"), "set_invulnerable", "is_invulnerable");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "collision_radius"), "set_collision_radius", "get_collision_radius");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "graze_radius"), "set_graze_radius", "get_graze_radius");

    ADD_SIGNAL(MethodInfo("hit"));
    ADD_SIGNAL(MethodInfo("graze"));
}

Hitbox::Hitbox() {
    collision_radius = 2;
    graze_radius = 16;
    invulnerable = false;
    colliding_shot = NULL;
    grazing_shot = NULL;
    danmaku = NULL;
}