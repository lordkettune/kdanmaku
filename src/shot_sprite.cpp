#include "shot_sprite.hpp"

using namespace godot;

void ShotSprite::_register_methods()
{
    register_property<ShotSprite, String>("key", &ShotSprite::key, "");
    register_property<ShotSprite, Ref<Texture>>("texture", &ShotSprite::texture, nullptr);
    register_property<ShotSprite, Rect2>("region", &ShotSprite::region, Rect2(0, 0, 0, 0));
}

void ShotSprite::_init()
{
    key = "";
    texture = nullptr;
    region = Rect2(0, 0, 0, 0);
}