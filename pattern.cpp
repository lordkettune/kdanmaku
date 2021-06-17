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

    Transform2D transform = get_global_transform();

    Rect2 region = danmaku->get_region();
    region = region.grow(despawn_distance + danmaku->get_tolerance());
    region = transform.xform_inv(region);

    Hitbox* hitbox = danmaku->get_hitbox();
    Vector2 hitbox_position = Vector2(0, 0);

    if (hitbox != NULL) {
        hitbox_position = hitbox->get_global_transform().get_origin();
        hitbox_position = transform.xform_inv(hitbox_position);
    }

    bool clean = false;

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

            // Run effects
            for (int j = 0; j != effect_count; ++j) {
                if (!effects[j].is_null()) {
                    effects[j]->execute(j, shot);
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
            float distance = shot->get_position().distance_to(hitbox_position);

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
        if (!region.has_point(shot->get_position())) {
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
    transform = danmaku->get_global_transform().inverse() * transform;

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

int Pattern::add_effect(const Ref<ShotEffect>& p_effect) {
    effects[effect_count] = p_effect;
    return effect_count++;
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

void Pattern::set_delegate(Object* p_delegate) {
    delegate = p_delegate;
}

Object* Pattern::get_delegate() const {
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
        case COUNT:    params.count = p_value;    break;
        case SHAPE:    params.shape = p_value;    break;
        case SPRITE:   params.sprite = p_value;   break;
        case OFFSET:   params.offset = p_value;   break;
        case EFFECTS:  params.effects = p_value;  break;
        case ROTATION: params.rotation = p_value; break;
        case SPEED:    params.speed = p_value;    break;
        case PAUSED:   params.paused = p_value;   break;
        case AIM:      params.aim = p_value;      break;
        default: registers[p_reg >> 2] = p_value; break;
    }
}

Variant Pattern::get_register(Register p_reg) const {
    switch (p_reg) {
        case COUNT:    return params.count;
        case SHAPE:    return params.shape;
        case SPRITE:   return params.sprite;
        case OFFSET:   return params.offset;
        case EFFECTS:  return params.effects;
        case ROTATION: return params.rotation;
        case SPEED:    return params.speed;
        case PAUSED:   return params.paused;
        case AIM:      return params.aim;
        default: return registers[p_reg >> 2];
    }
}

void Pattern::reset() {
    params.count = 1;
    params.shape = "";
    params.sprite = "";
    params.effects = Vector<int>();
    params.offset = Vector2(0, 0);
    params.rotation = 0;
    params.speed = 0;
    params.paused = false;
    params.aim = false;
}

void Pattern::fire() {
    ERR_FAIL_NULL(danmaku);

    void(Pattern::*shape)(Shot*) = &Pattern::shape_custom;
    if (params.shape.length()) {
        switch ((char)params.shape[0]) {
            case 's':
                if (params.shape == "single" ) shape = &Pattern::shape_single;
                if (params.shape == "single_layered") shape = &Pattern::shape_single_layered;
                break;
            case 'c':
                if (params.shape == "circle") shape = &Pattern::shape_circle;
                if (params.shape == "circle_layered") shape = &Pattern::shape_circle_layered;
                break;
            case 'f':
                if (params.shape == "fan") shape = &Pattern::shape_fan;
                if (params.shape == "fan_layered") shape = &Pattern::shape_fan_layered;
                break;
        }
    }

    Ref<ShotSprite> sprite = danmaku->get_sprite(params.sprite);
    if (sprite.is_null()) {
        ERR_FAIL_MSG("No sprite defined, cannot fire");
    }

    float rotation = params.rotation;
    if (params.aim) {
        Hitbox* hitbox = danmaku->get_hitbox();
        ERR_FAIL_NULL(hitbox);
        rotation += (hitbox->get_global_position() - get_global_position()).angle();
    }
    Vector2 direction = Vector2(Math::cos(rotation), Math::sin(rotation));

    for (int i = 0; i != params.count; ++i) {
        Shot* shot = danmaku->capture();
        shot->reset(this, i);
        shot->set_direction(direction);
        shot->set_sprite(sprite);
        shot->set_speed(params.speed);
        shot->set_position(params.offset);
        shot->set_effects(params.effects);
        shot->set_paused(params.paused);
        shot->flag(Shot::FLAG_ACTIVE);
        shots.push_back(shot);
        (this->*shape)(shot);
    }
}

void Pattern::shape_single(Shot* p_shot) {}

void Pattern::shape_circle(Shot* p_shot) {
    p_shot->set_rotation(p_shot->get_rotation() + p_shot->get_id() * (2 * Math_PI / params.count));
}

void Pattern::shape_fan(Shot* p_shot) {
    float angle = get_register(SHAPE0);
    float base = p_shot->get_rotation() - angle / 2;
    float theta = angle / (params.count - 1);

    p_shot->set_rotation(base + p_shot->get_id() * theta);
}

void Pattern::shape_single_layered(Shot* p_shot) {
    float step = get_register(SHAPE0);

    p_shot->set_speed(p_shot->get_speed() + step * p_shot->get_id());
}

void Pattern::shape_circle_layered(Shot* p_shot) {
    int layers = get_register(SHAPE0);
    float step = get_register(SHAPE1);

    int row = p_shot->get_id() % layers;
    int col = p_shot->get_id() / layers;

    p_shot->set_speed(p_shot->get_speed() + step * row);
    p_shot->set_rotation(p_shot->get_rotation() + col * (2 * Math_PI / (params.count / layers)));
}

void Pattern::shape_fan_layered(Shot* p_shot) {
    float angle = get_register(SHAPE0);
    float base = p_shot->get_rotation() - angle / 2;

    int layers = get_register(SHAPE1);
    float step = get_register(SHAPE2);

    float theta = angle / (params.count / layers - 1);
    int row = p_shot->get_id() % layers;
    int col = p_shot->get_id() / layers;

    p_shot->set_speed(p_shot->get_speed() + step * row);
    p_shot->set_rotation(base + col * theta);
}

void Pattern::shape_custom(Shot* p_shot) {
    // TODO
}

void Pattern::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_danmaku"), &Pattern::get_danmaku);
    ClassDB::bind_method(D_METHOD("play_sfx", "key"), &Pattern::play_sfx);
    ClassDB::bind_method(D_METHOD("set_register", "register", "value"), &Pattern::set_register);
    ClassDB::bind_method(D_METHOD("get_register", "register"), &Pattern::get_register);
    ClassDB::bind_method(D_METHOD("add_effect", "effect"), &Pattern::add_effect);
    ClassDB::bind_method(D_METHOD("reset"), &Pattern::reset);
    ClassDB::bind_method(D_METHOD("fire"), &Pattern::fire);

    ClassDB::bind_method(D_METHOD("set_delegate", "delegate"), &Pattern::set_delegate);
    ClassDB::bind_method(D_METHOD("set_despawn_distance", "despawn_distance"), &Pattern::set_despawn_distance);
    ClassDB::bind_method(D_METHOD("set_autodelete", "autodelete"), &Pattern::set_autodelete);

    ClassDB::bind_method(D_METHOD("get_delegate"), &Pattern::get_delegate);
    ClassDB::bind_method(D_METHOD("get_despawn_distance"), &Pattern::get_despawn_distance);
    ClassDB::bind_method(D_METHOD("get_autodelete"), &Pattern::get_autodelete);

    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "delegate"), "set_delegate", "get_delegate");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "despawn_distance"), "set_despawn_distance", "get_despawn_distance");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "autodelete"), "set_autodelete", "get_autodelete");

    BIND_CONSTANT(REG0);
    BIND_CONSTANT(REG1);
    BIND_CONSTANT(REG2);
    BIND_CONSTANT(REG3);
    BIND_CONSTANT(REG4);
    BIND_CONSTANT(REG5);
    BIND_CONSTANT(REG6);
    BIND_CONSTANT(REG7);

    BIND_CONSTANT(SHAPE0);
    BIND_CONSTANT(SHAPE1);
    BIND_CONSTANT(SHAPE2);
    BIND_CONSTANT(SHAPE3);

    BIND_CONSTANT(COUNT);
    BIND_CONSTANT(SHAPE);
    BIND_CONSTANT(SPRITE);
    BIND_CONSTANT(OFFSET);
    BIND_CONSTANT(EFFECTS);
    BIND_CONSTANT(ROTATION);
    BIND_CONSTANT(SPEED);
    BIND_CONSTANT(AIM);
}

Pattern::Pattern() {
    danmaku = NULL;
    delegate = NULL;
    despawn_distance = 0;
    autodelete = false;
    effect_count = 0;
    reset();

    for (int i = 0; i != MAX_SHOT_EFFECTS; ++i) {
        effects[i] = Ref<ShotEffect>();
    }
}