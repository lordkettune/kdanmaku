#include "shot_sprite.hpp"

using namespace godot;

void ShotSprite::_register_methods()
{
    register_property<ShotSprite, String>("key", &ShotSprite::key, "");
    register_property<ShotSprite, Ref<Texture>>("texture", &ShotSprite::texture, nullptr);
    register_property<ShotSprite, Rect2>("region", &ShotSprite::region, Rect2(0, 0, 0, 0));
    register_property<ShotSprite, float>("collider_radius", &ShotSprite::collider_radius, 16);
}

void ShotSprite::_init()
{
    key = "";
    texture = nullptr;
    region = Rect2(0, 0, 0, 0);
    collider_radius = 16;
}

void ShotSprite::draw_to(CanvasItem* canvas_item, Vector2 position)
{
    canvas_item->draw_texture_rect_region(texture, Rect2(position - region.size / 2.0f, region.size), region);
}