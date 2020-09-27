#ifndef __KD_SHOT_SPRITE_HPP__
#define __KD_SHOT_SPRITE_HPP__

#include <Godot.hpp>
#include <Texture.hpp>
#include <Resource.hpp>

namespace godot {

class ShotSprite : public Resource {
    GODOT_CLASS(ShotSprite, Resource)

public:
    String key;
    Ref<Texture> texture;
    Rect2 region;

    static void _register_methods();
    void _init();
};

}

#endif