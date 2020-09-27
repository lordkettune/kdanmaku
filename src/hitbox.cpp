#include "hitbox.hpp"

using namespace godot;

void Hitbox::_register_methods()
{
    register_property<Hitbox, float>("radius", &Hitbox::radius, 2);
    register_property<Hitbox, bool>("invulnerable", &Hitbox::invulnerable, false);

    register_method("_enter_tree", &Hitbox::_enter_tree);
    register_method("_exit_tree", &Hitbox::_exit_tree);

    register_method("get_danmaku", &Hitbox::get_danmaku);

    register_signal<Hitbox>("hit", Dictionary());
}

void Hitbox::_init()
{
    radius = 2;
    invulnerable = false;
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

void Hitbox::hit()
{
    if (invulnerable)
        return;
    emit_signal("hit");
}