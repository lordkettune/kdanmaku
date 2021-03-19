#include "pattern.h"
#include "hitbox.h"

#include "core/math/math_funcs.h"
#include "core/method_bind_ext.gen.inc"

void Pattern::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_ENTER_TREE: {
            Node* parent = this;
            while (parent) {
                danmaku = Object::cast_to<Danmaku>(parent);
                if (danmaku) {
                    set_z_index(danmaku->get_z_index());
                    danmaku->add_pattern(this);
                    break;
                }
                parent = parent->get_parent();
            }
        } break;

        case NOTIFICATION_READY: {
            set_physics_process_internal(true);
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

        case NOTIFICATION_INTERNAL_PHYSICS_PROCESS: {
            tick();
        } break;

        case NOTIFICATION_DRAW: {
            draw();
        } break;
    }
}

void Pattern::tick() {
    ERR_FAIL_COND(danmaku == NULL);

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

        // Move shot by its direction and speed, then update its global position
        shot->set_position(shot->get_position() + shot->get_velocity());

        // Run effects
        for (int j = 0; j != effect_count; ++j) {
            if (!effects[j].is_null()) {
                effects[j]->execute(j, shot);
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

void Pattern::draw() {
    for (int i = 0; i != shots.size(); ++i) {
        Shot* shot = shots[i];

        Ref<ShotSprite> sprite = danmaku->get_sprite(shot->get_sprite_id());
        float rotation_radians = sprite->get_rotation_degrees() * Math_PI / 180.0f;

        if (sprite->get_face_motion()) {
            draw_set_transform(shot->get_position(), shot->get_rotation() + rotation_radians, Vector2(1, 1));
        } else {
            draw_set_transform(shot->get_position(), rotation_radians, Vector2(1, 1));
        }

        Rect2 region = sprite->get_region();
        draw_texture_rect_region(sprite->get_texture(), Rect2(region.size * -0.5f, region.size), region);
    }
}

void Pattern::set_register(Register p_reg, const Variant& p_value) {
    registers[p_reg >> 2] = p_value;
}

Variant Pattern::get_register(Register p_reg) const {
    return registers[p_reg >> 2];
}

int Pattern::add_effect(const Ref<ShotEffect>& p_effect) {
    effects[effect_count] = p_effect;
    return effect_count++;
}

Danmaku* Pattern::get_danmaku() const {
    return danmaku;
}

void Pattern::single(Dictionary p_override) {
    pattern(1, p_override, [](Shot* p_shot) {});
}

void Pattern::layered(int p_layers, float p_min, float p_max, Dictionary p_override) {
    float step = (p_max - p_min) / (p_layers - 1);

    pattern(p_layers, p_override, [=](Shot* p_shot) {
        p_shot->set_speed(p_min + step * p_shot->get_id());
    });
}

void Pattern::circle(int p_count, Dictionary p_override) {
    pattern(p_count, p_override, [=](Shot* p_shot) {
        p_shot->set_direction(p_shot->get_direction().rotated(p_shot->get_id() * (Math_TAU / (float)p_count)));
    });
}

void Pattern::layered_circle(int p_count, int p_layers, float p_min, float p_max, Dictionary p_override) {
    float step = (p_max - p_min) / (p_layers - 1);

    pattern(p_count * p_layers, p_override, [=](Shot* p_shot) {
        int col = p_shot->get_id() % p_count;
        int row = p_shot->get_id() / p_count;
        p_shot->set_direction(p_shot->get_direction().rotated(col * (Math_TAU / (float)p_count)));
        p_shot->set_speed(p_min + step * row);
    });
}

void Pattern::fan(int p_count, float p_theta, Dictionary p_override) {
    float base = -p_theta * 0.5f;
    float step = p_theta / (p_count - 1);

    pattern(p_count, p_override, [=](Shot* p_shot) {
        p_shot->set_direction(p_shot->get_direction().rotated(base + step * p_shot->get_id()));
    });
}

void Pattern::layered_fan(int p_count, float p_theta, int p_layers, float p_min, float p_max, Dictionary p_override) {
    float a_base = -p_theta * 0.5f;
    float a_step = p_theta / (p_count - 1);
    float s_step = (p_max - p_min) / (p_layers - 1);

    pattern(p_count * p_layers, p_override, [=](Shot* p_shot) {
        int col = p_shot->get_id() % p_count;
        int row = p_shot->get_id() / p_count;
        p_shot->set_direction(p_shot->get_direction().rotated(a_base + a_step * col));
        p_shot->set_speed(p_min + s_step * row);
    });
}

void Pattern::custom(int p_count, String p_name, Dictionary p_override) {
    if (delegate == NULL) {
        ERR_PRINT("Pattern does not have a delegate, can't fire custom pattern!");
        return;
    }

    if (!delegate->has_method(p_name)) {
        ERR_PRINT("Delegate has no custom pattern by name \"" + p_name + "\"");
        return;
    }

    pattern(p_count, p_override, [=](Shot* p_shot) {
        delegate->call(p_name, p_shot, p_count);
    });
}

void Pattern::set_delegate(Object* p_delegate) {
    delegate = p_delegate;
}

Object* Pattern::get_delegate() const {
    return delegate;
}

void Pattern::set_parameters(const Dictionary& p_parameters) {
    parameters = p_parameters;
}

Dictionary Pattern::get_parameters() const {
    return parameters;
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

void Pattern::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_danmaku"), &Pattern::get_danmaku);
    ClassDB::bind_method(D_METHOD("add_effect", "effect"), &Pattern::add_effect);

    ClassDB::bind_method(D_METHOD("single"), &Pattern::single);
    ClassDB::bind_method(D_METHOD("circle"), &Pattern::circle);
    ClassDB::bind_method(D_METHOD("layered"), &Pattern::layered);
    ClassDB::bind_method(D_METHOD("layered_circle"), &Pattern::layered_circle);
    ClassDB::bind_method(D_METHOD("fan"), &Pattern::fan);
    ClassDB::bind_method(D_METHOD("layered_fan"), &Pattern::layered_fan);
    ClassDB::bind_method(D_METHOD("custom"), &Pattern::custom);

    ClassDB::bind_method(D_METHOD("set_delegate", "delegate"), &Pattern::set_delegate);
    ClassDB::bind_method(D_METHOD("set_parameters", "parameters"), &Pattern::set_parameters);
    ClassDB::bind_method(D_METHOD("set_despawn_distance", "despawn_distance"), &Pattern::set_despawn_distance);
    ClassDB::bind_method(D_METHOD("set_autodelete", "autodelete"), &Pattern::set_autodelete);

    ClassDB::bind_method(D_METHOD("get_delegate"), &Pattern::get_delegate);
    ClassDB::bind_method(D_METHOD("get_parameters"), &Pattern::get_parameters);
    ClassDB::bind_method(D_METHOD("get_despawn_distance"), &Pattern::get_despawn_distance);
    ClassDB::bind_method(D_METHOD("get_autodelete"), &Pattern::get_autodelete);

    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "delegate"), "set_delegate", "get_delegate");
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "parameters"), "set_parameters", "get_parameters");
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
}

Pattern::Pattern() {
    danmaku = NULL;
    delegate = NULL;
    despawn_distance = 0;
    autodelete = false;
    parameters = Dictionary();
    effect_count = 0;

    for (int i = 0; i != MAX_SHOT_EFFECTS; ++i) {
        effects[i] = Ref<ShotEffect>();
    }
}