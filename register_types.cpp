#include "register_types.h"

#include "frames.h"
#include "shot_sprite.h"
#include "shot.h"
#include "shot_effect.h"
#include "hitbox.h"
#include "danmaku.h"
#include "pattern.h"

void register_kdanmaku_types() {
    ClassDB::register_class<Frames>();
    ClassDB::register_class<ShotSprite>();
    ClassDB::register_class<Shot>();
    ClassDB::register_class<ShotEffect>();
    ClassDB::register_class<Hitbox>();
    ClassDB::register_class<Danmaku>();
    ClassDB::register_class<Pattern>();
}

void unregister_kdanmaku_types() {
}