#include "register_types.h"

#include "frames.h"
#include "shot_sprite.h"
#include "shot.h"
#include "shot_effect.h"

void register_kdanmaku_types() {
    ClassDB::register_class<Frames>();
    ClassDB::register_class<ShotSprite>();
    ClassDB::register_class<Shot>();
    ClassDB::register_class<ShotEffect>();
}

void unregister_kdanmaku_types() {
}