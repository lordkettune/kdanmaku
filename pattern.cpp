#include "pattern.h"
#include "hitbox.h"

#include "core/math/math_funcs.h"

void Pattern::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_ENTER_TREE: {
            Node* parent = this;
            while (parent) {
                danmaku = Object::cast_to<Danmaku>(parent);
                if (danmaku) {
                    danmaku->add_pattern(this);
                    break;
                }
                parent = parent->get_parent();
            }
            set_physics_process(true);
        } break;

        case NOTIFICATION_EXIT_TREE: {
            if (danmaku) {
                for (int i = 0; i != shots.size(); ++i) {
                    danmaku->release(shots[i]);
                }
                shots.resize(0);
                danmaku->remove_pattern(this);
            }
        } break;

        case NOTIFICATION_PHYSICS_PROCESS: {
            _tick();
        } break;
    }
}

void Pattern::_tick() {
    ERR_FAIL_NULL(danmaku);

    if (autodelete && shots.empty()) {
        queue_delete();
        return;
    }

    bool clean = false;
    Rect2 region = danmaku->get_region().grow(despawn_distance + danmaku->get_tolerance());
    Hitbox* hitbox = danmaku->get_hitbox();

    Vector2 hitbox_position = Vector2(0, 0);
    if (hitbox) {
        hitbox_position = hitbox->get_global_transform().get_origin();
    }

    // Update all shots
    for (int i = 0; i != shots.size(); ++i) {
        Shot* shot = shots[i];

        if (!shot->flagged(Shot::FLAG_ACTIVE)) {
            clean = true;
            continue;
        }

        if (!shot->flagged(Shot::FLAG_PAUSED)) {
            // Move shot by its direction and speed, then update its global position
            shot->set_position(shot->get_position() + shot->get_velocity());

            // Run effect
            if (!shot->flagged(Shot::FLAG_CLEARED)) {
                if (shot->get_effect().is_valid()) {
                    shot->get_effect()->execute(shot);
                }
            }
        }

        // Process animations
        ShotFrame* frame = shot->get_frame();
        if (--frame->delay <= 0) {
            if (frame->cleared) {
                shot->unflag(Shot::FLAG_ACTIVE);
                clean = true;
            } else {
                *frame = shot->get_sprite()->get_frame(frame->next);
            }
        }

        // Check for graze or collision
        if (hitbox != NULL) {
            float distance = shot->get_global_position().distance_to(hitbox_position);

            if (distance <= hitbox->get_collision_radius() + shot->get_radius()) {
                if (!shot->flagged(Shot::FLAG_COLLIDING)) {
                    hitbox->hit(shot);
                    shot->flag(Shot::FLAG_COLLIDING);
                }
            } else {
                shot->unflag(Shot::FLAG_COLLIDING);
            }

            if (distance <= hitbox->get_graze_radius() + shot->get_radius()) {
                if (!shot->flagged(Shot::FLAG_GRAZING)) {
                    hitbox->graze(shot);
                    shot->flag(Shot::FLAG_GRAZING);
                }
            } else {
                shot->unflag(Shot::FLAG_GRAZING);
            }
        }

        // Clear shot if it's either outside the gameplay region or in clear circle
        if (!region.has_point(shot->get_global_position())) {
            shot->unflag(Shot::FLAG_ACTIVE);
            clean = true;
        }
    }

    // Shots left danmaku region, release them back to Danmaku
    if (clean) {
        for (int i = 0; i != shots.size();) {
            if (!shots[i]->flagged(Shot::FLAG_ACTIVE)) {
                danmaku->release(shots[i]);
                shots.remove(i);
            } else {
                i++;
            }
        }
    }

    update();
}

int Pattern::fill_buffer(real_t*& buf) {
    ERR_FAIL_NULL_V(danmaku, 0);

    Transform2D transform = get_global_transform();
    transform = danmaku->get_global_transform().affine_inverse() * transform;

    Ref<Texture> atlas = danmaku->get_atlas();
    if (!atlas.is_valid()) {
        return 0;
    }
    Size2 atlas_size = atlas->get_size();

    for (int i = 0; i != shots.size(); ++i) {
        Vector2 position = transform.xform(shots[i]->get_position());
        ShotFrame* frame = shots[i]->get_frame();

        Vector2 x = Vector2(1, 0);
        Vector2 y = Vector2(0, 1);
        if (frame->face_motion) {
            x = shots[i]->get_direction();
            y = Vector2(-x.y, x.x);
        }

        Rect2 region = frame->region;

        buf[0] = x.x * region.size.width / 2;
        buf[1] = y.x * region.size.height / 2;
        buf[2] = 0;
        buf[3] = position.x;
        buf[4] = x.y * region.size.width / 2;
        buf[5] = y.y * region.size.height / 2;
        buf[6] = 0;
        buf[7] = position.y;

        buf[8] = region.size.width / atlas_size.width;
        buf[9] = region.size.height / atlas_size.height;
        buf[10] = region.position.x / atlas_size.width;
        buf[11] = region.position.y / atlas_size.height;

        buf += (8 + 4);
    }

    return shots.size();
}

void Pattern::play_sfx(const StringName& p_key) {
    ERR_FAIL_NULL(danmaku);
    danmaku->play_sfx(p_key);
}

Danmaku* Pattern::get_danmaku() const {
    return danmaku;
}

void Pattern::remove_from_danmaku() {
    ERR_FAIL_NULL(danmaku);
    for (int i = 0; i != shots.size(); ++i) {
        danmaku->release(shots[i]);
    }
    shots.resize(0);
    danmaku->remove_pattern(this);
    danmaku = nullptr;
}

void Pattern::set_delegate(Ref<Reference> p_delegate) {
    delegate = p_delegate;
}
Ref<Reference> Pattern::get_delegate() const {
    return delegate;
}

void Pattern::set_despawn_distance(float p_despawn_distance) {
    despawn_distance = p_despawn_distance;
}
float Pattern::get_despawn_distance() const {
    return despawn_distance;
}

void Pattern::set_autodelete(bool p_autodelete) {
    autodelete = p_autodelete;
}
bool Pattern::get_autodelete() const {
    return autodelete;
}

void Pattern::set_register(Register p_reg, const Variant& p_value) {
    switch (p_reg) {
        case FIRE_COUNT:    set_fire_count(p_value);    break;
        case FIRE_SHAPE:    set_fire_shape(p_value);    break;
        case FIRE_SPRITE:   set_fire_sprite(p_value);   break;
        case FIRE_OFFSET:   set_fire_offset(p_value);   break;
        case FIRE_EFFECT:   set_fire_effect(p_value);   break;
        case FIRE_ROTATION: set_fire_rotation(p_value); break;
        case FIRE_SPEED:    set_fire_speed(p_value);    break;
        case FIRE_PAUSED:   set_fire_paused(p_value);   break;
        case FIRE_AIM:      set_fire_aim(p_value);      break;
        default: registers[p_reg >> 2] = p_value;  break;
    }
}

Variant Pattern::get_register(Register p_reg) const {
    switch (p_reg) {
        case FIRE_COUNT:    return get_fire_count();
        case FIRE_SHAPE:    return get_fire_shape();
        case FIRE_SPRITE:   return get_fire_sprite();
        case FIRE_OFFSET:   return get_fire_offset();
        case FIRE_EFFECT:   return get_fire_effect();
        case FIRE_ROTATION: return get_fire_rotation();
        case FIRE_SPEED:    return get_fire_speed();
        case FIRE_PAUSED:   return get_fire_paused();
        case FIRE_AIM:      return get_fire_aim();
        default: return registers[p_reg >> 2];
    }
}

void Pattern::set_fire_count(int p_count) {
    fire_params.count = p_count;
}
int Pattern::get_fire_count() const {
    return fire_params.count;
}

void Pattern::set_fire_shape(String p_shape) {
    fire_params.shape = p_shape;
}
String Pattern::get_fire_shape() const {
    return fire_params.shape;
}

void Pattern::set_fire_sprite(String p_sprite) {
    fire_params.sprite = p_sprite;
}
String Pattern::get_fire_sprite() const {
    return fire_params.sprite;
}

void Pattern::set_fire_offset(Vector2 p_offset) {
    fire_params.offset = p_offset;
}
Vector2 Pattern::get_fire_offset() const {
    return fire_params.offset;
}

void Pattern::set_fire_effect(Ref<ShotEffect> p_effect) {
    fire_params.effect = p_effect;
}
Ref<ShotEffect> Pattern::get_fire_effect() const {
    return fire_params.effect;
}

void Pattern::set_fire_rotation(float p_rotation) {
    fire_params.rotation = p_rotation;
}
float Pattern::get_fire_rotation() const {
    return fire_params.rotation;
}

void Pattern::set_fire_speed(float p_speed) {
    fire_params.speed = p_speed;
}
float Pattern::get_fire_speed() const {
    return fire_params.speed;
}

void Pattern::set_fire_paused(bool p_paused) {
    fire_params.paused = p_paused;
}
bool Pattern::get_fire_paused() const {
    return fire_params.paused;
}

void Pattern::set_fire_aim(bool p_aim) {
    fire_params.aim = p_aim;
}
bool Pattern::get_fire_aim() const {
    return fire_params.aim;
}

void Pattern::reset() {
    fire_params.count = 1;
    fire_params.shape = "single";
    fire_params.sprite = "";
    fire_params.effect = Ref<ShotEffect>();
    fire_params.offset = Vector2(0, 0);
    fire_params.rotation = 0;
    fire_params.speed = 0;
    fire_params.paused = false;
    fire_params.aim = false;
}

void Pattern::fire() {
    ERR_FAIL_NULL(danmaku);

    void(Pattern::*shape)(Shot*) = &Pattern::shape_custom;
    if (fire_params.shape.length()) {
        switch ((char)fire_params.shape[0]) {
            case 's':
                if (fire_params.shape == "single") shape = &Pattern::shape_single;
                if (fire_params.shape == "single_layered") shape = &Pattern::shape_single_layered;
                break;
            case 'c':
                if (fire_params.shape == "circle") shape = &Pattern::shape_circle;
                if (fire_params.shape == "circle_layered") shape = &Pattern::shape_circle_layered;
                break;
            case 'f':
                if (fire_params.shape == "fan") shape = &Pattern::shape_fan;
                if (fire_params.shape == "fan_layered") shape = &Pattern::shape_fan_layered;
                break;
        }
    }

    Ref<ShotSprite> sprite = danmaku->get_sprite(fire_params.sprite);
    if (sprite.is_null()) {
        ERR_FAIL_MSG("No sprite defined, cannot fire");
    }

    float rotation = fire_params.rotation;
    if (fire_params.aim) {
        Hitbox* hitbox = danmaku->get_hitbox();
        ERR_FAIL_NULL(hitbox);
        rotation += (hitbox->get_global_position() - get_global_position()).angle();
    }
    Vector2 direction = Vector2(Math::cos(rotation), Math::sin(rotation));

    for (int i = 0; i != fire_params.count; ++i) {
        Shot* shot = danmaku->capture();
        shot->reset(this, i);
        shot->set_direction(direction);
        shot->set_sprite(sprite);
        shot->set_speed(fire_params.speed);
        shot->set_position(fire_params.offset);
        shot->set_effect(fire_params.effect);
        shot->set_paused(fire_params.paused);
        shot->flag(Shot::FLAG_ACTIVE);
        shots.push_back(shot);
        (this->*shape)(shot);
    }

    reset();
}

void Pattern::fire_single() {
    set_fire_shape("single");
    fire();
}

void Pattern::fire_circle() {
    set_fire_shape("circle");
    fire();
}

void Pattern::fire_fan(float p_angle) {
    set_fire_shape("fan");
    set_register(FIRE_SHAPE0, p_angle);
    fire();
}

void Pattern::fire_single_layered(float p_step) {
    set_fire_shape("single_layered");
    set_register(FIRE_SHAPE0, p_step);
    fire();
}

void Pattern::fire_circle_layered(int p_layers, float p_step) {
    set_fire_shape("circle_layered");
    set_register(FIRE_SHAPE0, p_layers);
    set_register(FIRE_SHAPE1, p_step);
    fire();
}

void Pattern::fire_fan_layered(float p_angle, int p_layers, float p_step) {
    set_fire_shape("fan_layered");
    set_register(FIRE_SHAPE0, p_angle);
    set_register(FIRE_SHAPE1, p_layers);
    set_register(FIRE_SHAPE2, p_step);
    fire();
}

void Pattern::fire_custom(String p_name, Variant p_s0, Variant p_s1, Variant p_s2, Variant p_s3) {
    set_fire_shape(p_name);
    set_register(FIRE_SHAPE0, p_s0);
    set_register(FIRE_SHAPE1, p_s1);
    set_register(FIRE_SHAPE2, p_s2);
    set_register(FIRE_SHAPE3, p_s3);
    fire();    
}

void Pattern::shape_single(Shot* p_shot) {}

void Pattern::shape_circle(Shot* p_shot) {
    p_shot->set_rotation(p_shot->get_rotation() + p_shot->get_id() * (2 * Math_PI / fire_params.count));
}

void Pattern::shape_fan(Shot* p_shot) {
    float angle = get_register(FIRE_SHAPE0);
    float base = p_shot->get_rotation() - angle / 2;
    float theta = angle / (fire_params.count - 1);

    p_shot->set_rotation(base + p_shot->get_id() * theta);
}

void Pattern::shape_single_layered(Shot* p_shot) {
    float step = get_register(FIRE_SHAPE0);

    p_shot->set_speed(p_shot->get_speed() + step * p_shot->get_id());
}

void Pattern::shape_circle_layered(Shot* p_shot) {
    int layers = get_register(FIRE_SHAPE0);
    float step = get_register(FIRE_SHAPE1);

    int row = p_shot->get_id() % layers;
    int col = p_shot->get_id() / layers;

    p_shot->set_speed(p_shot->get_speed() + step * row);
    p_shot->set_rotation(p_shot->get_rotation() + col * (2 * Math_PI / (fire_params.count / layers)));
}

void Pattern::shape_fan_layered(Shot* p_shot) {
    float angle = get_register(FIRE_SHAPE0);
    float base = p_shot->get_rotation() - angle / 2;

    int layers = get_register(FIRE_SHAPE1);
    float step = get_register(FIRE_SHAPE2);

    float theta = angle / (fire_params.count / layers - 1);
    int row = p_shot->get_id() % layers;
    int col = p_shot->get_id() / layers;

    p_shot->set_speed(p_shot->get_speed() + step * row);
    p_shot->set_rotation(base + col * theta);
}

void Pattern::shape_custom(Shot* p_shot) {
    ERR_FAIL_COND(delegate.is_null());
    Variant shot = p_shot;
    Variant::CallError error;
    const Variant* argv[5] = {&shot, &registers[FIRE_SHAPE0 >> 2], &registers[FIRE_SHAPE1 >> 2], &registers[FIRE_SHAPE2 >> 2], &registers[FIRE_SHAPE3 >> 2]};
    int argc = 1;
    for (int i = 1; i != 5; ++i) {
        if (argv[i]->get_type() != Variant::Type::NIL) {
            argc++;
        }
    }
    delegate->call(fire_params.shape, argv, argc, error);
    if (error.error != Variant::CallError::CALL_OK) {
        ERR_FAIL_MSG("Failed to call custom pattern: " + Variant::get_call_error_text(this, fire_params.shape, argv, argc, error));
    }
}

void Pattern::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_danmaku"), &Pattern::get_danmaku);
    ClassDB::bind_method(D_METHOD("play_sfx", "key"), &Pattern::play_sfx);
    ClassDB::bind_method(D_METHOD("set_register", "register", "value"), &Pattern::set_register);
    ClassDB::bind_method(D_METHOD("get_register", "register"), &Pattern::get_register);
    ClassDB::bind_method(D_METHOD("reset"), &Pattern::reset);

    ClassDB::bind_method(D_METHOD("fire"), &Pattern::fire);
    ClassDB::bind_method(D_METHOD("fire_single"), &Pattern::fire_single);
    ClassDB::bind_method(D_METHOD("fire_circle"), &Pattern::fire_circle);
    ClassDB::bind_method(D_METHOD("fire_fan"), &Pattern::fire_fan);
    ClassDB::bind_method(D_METHOD("fire_single_layered"), &Pattern::fire_single_layered);
    ClassDB::bind_method(D_METHOD("fire_circle_layered"), &Pattern::fire_circle_layered);
    ClassDB::bind_method(D_METHOD("fire_fan_layered"), &Pattern::fire_fan_layered);
    ClassDB::bind_method(D_METHOD("fire_custom", "name", "p0", "p1", "p2", "p3"), &Pattern::fire_custom, DEFVAL(Variant()), DEFVAL(Variant()), DEFVAL(Variant()), DEFVAL(Variant()));

    ClassDB::bind_method(D_METHOD("set_fire_count", "count"), &Pattern::set_fire_count);
    ClassDB::bind_method(D_METHOD("set_fire_shape", "shape"), &Pattern::set_fire_shape);
    ClassDB::bind_method(D_METHOD("set_fire_sprite", "sprite"), &Pattern::set_fire_sprite);
    ClassDB::bind_method(D_METHOD("set_fire_offset", "offset"), &Pattern::set_fire_offset);
    ClassDB::bind_method(D_METHOD("set_fire_effect", "effect"), &Pattern::set_fire_effect);
    ClassDB::bind_method(D_METHOD("set_fire_rotation", "rotation"), &Pattern::set_fire_rotation);
    ClassDB::bind_method(D_METHOD("set_fire_speed", "speed"), &Pattern::set_fire_speed);
    ClassDB::bind_method(D_METHOD("set_fire_paused", "paused"), &Pattern::set_fire_paused);
    ClassDB::bind_method(D_METHOD("set_fire_aim", "aim"), &Pattern::set_fire_aim);

    ClassDB::bind_method(D_METHOD("get_fire_count"), &Pattern::get_fire_count);
    ClassDB::bind_method(D_METHOD("get_fire_shape"), &Pattern::get_fire_shape);
    ClassDB::bind_method(D_METHOD("get_fire_sprite"), &Pattern::get_fire_sprite);
    ClassDB::bind_method(D_METHOD("get_fire_offset"), &Pattern::get_fire_offset);
    ClassDB::bind_method(D_METHOD("get_fire_effect"), &Pattern::get_fire_effect);
    ClassDB::bind_method(D_METHOD("get_fire_rotation"), &Pattern::get_fire_rotation);
    ClassDB::bind_method(D_METHOD("get_fire_speed"), &Pattern::get_fire_speed);
    ClassDB::bind_method(D_METHOD("get_fire_paused"), &Pattern::get_fire_paused);
    ClassDB::bind_method(D_METHOD("get_fire_aim"), &Pattern::get_fire_aim);

    ClassDB::bind_method(D_METHOD("set_delegate", "delegate"), &Pattern::set_delegate);
    ClassDB::bind_method(D_METHOD("set_despawn_distance", "despawn_distance"), &Pattern::set_despawn_distance);
    ClassDB::bind_method(D_METHOD("set_autodelete", "autodelete"), &Pattern::set_autodelete);

    ClassDB::bind_method(D_METHOD("get_delegate"), &Pattern::get_delegate);
    ClassDB::bind_method(D_METHOD("get_despawn_distance"), &Pattern::get_despawn_distance);
    ClassDB::bind_method(D_METHOD("get_autodelete"), &Pattern::get_autodelete);

    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "delegate"), "set_delegate", "get_delegate");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "despawn_distance"), "set_despawn_distance", "get_despawn_distance");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "autodelete"), "set_autodelete", "get_autodelete");

    ADD_PROPERTY(PropertyInfo(Variant::INT, "fire_count"), "set_fire_count", "get_fire_count");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "fire_shape"), "set_fire_shape", "get_fire_shape");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "fire_sprite"), "set_fire_sprite", "get_fire_sprite");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "fire_offset"), "set_fire_offset", "get_fire_offset");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "fire_effect"), "set_fire_effect", "get_fire_effect");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "fire_rotation"), "set_fire_rotation", "get_fire_rotation");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "fire_speed"), "set_fire_speed", "get_fire_speed");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "fire_paused"), "set_fire_paused", "get_fire_paused");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "fire_aim"), "set_fire_aim", "get_fire_aim");

    BIND_CONSTANT(REG0);
    BIND_CONSTANT(REG1);
    BIND_CONSTANT(REG2);
    BIND_CONSTANT(REG3);
    BIND_CONSTANT(REG4);
    BIND_CONSTANT(REG5);
    BIND_CONSTANT(REG6);
    BIND_CONSTANT(REG7);

    BIND_CONSTANT(FIRE_SHAPE0);
    BIND_CONSTANT(FIRE_SHAPE1);
    BIND_CONSTANT(FIRE_SHAPE2);
    BIND_CONSTANT(FIRE_SHAPE3);

    BIND_CONSTANT(FIRE_COUNT);
    BIND_CONSTANT(FIRE_SHAPE);
    BIND_CONSTANT(FIRE_SPRITE);
    BIND_CONSTANT(FIRE_OFFSET);
    BIND_CONSTANT(FIRE_EFFECT);
    BIND_CONSTANT(FIRE_ROTATION);
    BIND_CONSTANT(FIRE_SPEED);
    BIND_CONSTANT(FIRE_PAUSED);
    BIND_CONSTANT(FIRE_AIM);
}

Pattern::Pattern() {
    danmaku = NULL;
    delegate = Ref<Reference>();
    despawn_distance = 0;
    autodelete = false;
    effect_count = 0;

    reset();
}