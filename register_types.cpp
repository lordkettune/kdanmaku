#include "register_types.h"

#include "frames.h"
#include "shot_sprite.h"

void register_kdanmaku_types() {
    ClassDB::register_class<Frames>();
    ClassDB::register_class<ShotSprite>();
}

void unregister_kdanmaku_types() {
}