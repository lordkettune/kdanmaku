#include <Godot.hpp>

#include "shot.hpp"
#include "danmaku.hpp"
#include "pattern.hpp"
#include "shot_sprite.hpp"
#include "hitbox.hpp"
#include "parser.hpp"

using namespace godot;

bool time_range(Shot* shot, int start, int end)
{
    return shot->time >= start && shot->time < end;
}

extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options* o)
{
    Godot::gdnative_init(o);

    Parser* parser = Parser::get_singleton();

    parser->register_selector(time_range, "time_range");
}

extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options* o)
{
    Godot::gdnative_terminate(o);

    Parser::free_singleton();
}

extern "C" void GDN_EXPORT godot_nativescript_init(void* handle)
{
    Godot::nativescript_init(handle);

    register_class<Shot>();
    register_class<ShotSprite>();
    register_class<Danmaku>();
    register_class<Hitbox>();
    register_class<Pattern>();
}