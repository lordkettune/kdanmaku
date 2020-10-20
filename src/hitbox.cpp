#include "hitbox.hpp"

using namespace godot;

void Hitbox::_register_methods()
{
    register_property<Hitbox, float>("collision_radius", &Hitbox::collision_radius, 2);
    register_property<Hitbox, float>("graze_radius", &Hitbox::graze_radius, 16);
    register_property<Hitbox, bool>("invulnerable", &Hitbox::invulnerable, false);

    register_method("_enter_tree", &Hitbox::_enter_tree);
    register_method("_exit_tree", &Hitbox::_exit_tree);

    register_method("get_danmaku", &Hitbox::get_danmaku);
    register_method("get_colliding_shot", &Hitbox::get_colliding_shot);
    register_method("get_grazing_shot", &Hitbox::get_grazing_shot);

    register_signal<Hitbox>("hit", Dictionary());
    register_signal<Hitbox>("graze", Dictionary());
}

void Hitbox::_init()
{
    collision_radius = 2;
    graze_radius = 16;
    invulnerable = false;
    _colliding_shot = nullptr;
    _grazing_shot = nullptr;
    _danmaku = nullptr;
}

void Hitbox::_enter_tree()
{
    Node* parent = get_parent();
    while ((bool)parent->call("is_danmaku") != true) {
        parent = parent->get_parent();
        if (parent == nullptr) {
            api->godot_print_error("Hitbox is not a child of Danmaku!", "_enter_tree", __FILE__, __LINE__);
            return;
        }
    }
    _danmaku = Object::cast_to<Danmaku>(parent);
    _danmaku->register_hitbox(this);
}

void Hitbox::_exit_tree()
{
    _danmaku->remove_hitbox();
}

void Hitbox::hit(Shot* shot)
{
    if (invulnerable)
        return;
    _colliding_shot = shot;
    emit_signal("hit");
}

void Hitbox::graze(Shot* shot)
{
    if (invulnerable)
        return;
    _grazing_shot = shot;
    emit_signal("graze");
}