#include "register_types.h"
#include "core/object/class_db.h"

#include "frames.h"

void register_kdanmaku_types() {
    ClassDB::register_class<Frames>();
}

void unregister_kdanmaku_types() {
}