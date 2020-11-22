// ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// *:･ﾟ✧ init.cpp *:･ﾟ✧
// 
// Entry point -- this is where GDNative is set up. Registers object types.
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

#include <Godot.hpp>

#include "shot.hpp"
#include "danmaku.hpp"
#include "pattern.hpp"
#include "shot_sprite.hpp"
#include "hitbox.hpp"
#include "parser.hpp"
#include "frames.hpp"
#include "standard_lib.hpp"

using namespace godot;

extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options* p_options) {
    Godot::gdnative_init(p_options);
    
    Parser::get_singleton();
    register_standard_lib();
}

extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options* p_options) {
    Godot::gdnative_terminate(p_options);
    
    Parser::free_singleton();
}

extern "C" void GDN_EXPORT godot_nativescript_init(void* p_handle) {
    Godot::nativescript_init(p_handle);

    register_class<Shot>();
    register_class<ShotSprite>();
    register_class<Danmaku>();
    register_class<Hitbox>();
    register_class<Pattern>();
    register_class<Frames>();
}