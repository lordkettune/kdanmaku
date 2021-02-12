#include "pattern.hpp"

#include <Math.hpp>

#include "hitbox.hpp"

using namespace godot;

void Pattern::_enter_tree() {
    Node* parent = get_parent();
    while ((bool)parent->call("is_danmaku") != true) {
        parent = parent->get_parent();
        if (parent == nullptr) {
            Godot::print_error("Pattern is not a descendent of a Danmaku node!", "_enter_tree", __FILE__, __LINE__);
            return;
        }
    }
    danmaku = Object::cast_to<Danmaku>(parent);
    danmaku->add_pattern(this);
    set_z_index(danmaku->get_z_index());
}

void Pattern::_exit_tree() {
    // Release all shot IDs when exiting tree
    for (Shot* shot : shots) {
        danmaku->release(shot);
    }
    danmaku->remove_pattern(this);
    shots.clear();
}

void Pattern::_physics_process(float p_delta) {
    ERR_FAIL_COND(danmaku == nullptr);

    Transform2D transform = get_global_transform();
    Rect2 region = danmaku->get_region();

    Hitbox* hitbox = danmaku->get_hitbox();
    Vector2 hitbox_pos = Vector2(0, 0);
    if (hitbox != nullptr) {
        hitbox_pos = hitbox->get_global_transform().get_origin();
    }

    int inactive = 0;

    // Update all shots
    for (Shot* shot : shots) {
        if (!shot->flagged(Shot::FLAG_ACTIVE)) {
            ++inactive;
            continue;
        }

        // Move shot by its direction and speed, then update its global position
        shot->position += shot->direction * shot->speed;
        shot->global_position = transform.xform(shot->position);

        if (has_effects) {
            for (int i = 0; i != MAX_EFFECTS; ++i) {
                if (effects[i] != nullptr && shot->has_effect(i)) {
                    effects[i]->execute(shot);
                }
            }
        }

        ++shot->time;

        // Check for graze or collision
        if (hitbox != nullptr) {
            float distance = shot->global_position.distance_to(hitbox_pos);

            if (distance <= hitbox->get_collision_radius() + shot->radius) {
                if (!shot->flagged(Shot::FLAG_COLLIDING)) {
                    hitbox->hit(shot);
                    shot->flag(Shot::FLAG_COLLIDING);
                }
            } else {
                shot->unflag(Shot::FLAG_COLLIDING);
            }

            if (distance <= hitbox->get_graze_radius() + shot->radius) {
                if (!shot->flagged(Shot::FLAG_GRAZING)) {
                    hitbox->graze(shot);
                    shot->flag(Shot::FLAG_GRAZING);
                }
            } else {
                shot->unflag(Shot::FLAG_GRAZING);
            }
        }

        // Clear shot if it's either outside the gameplay region or in clear circle
        if (!region.has_point(shot->global_position)) {
            shot->unflag(Shot::FLAG_ACTIVE);
            ++inactive;
        }
    }

    // Shots left danmaku region, release them back to Danmaku
    if (inactive) {
        for (int i = 0; i != shots.size();) {
            if (!shots[i]->flagged(Shot::FLAG_ACTIVE)) {
                danmaku->release(shots[i]);
                shots.erase(shots.begin() + i);
            } else {
                i++;
            }
        }
    }

    update();
}

void Pattern::_draw() {
    for (Shot* shot : shots) {
        Ref<ShotSprite> sprite = danmaku->get_sprite(shot->sprite_id);
        float rotation_radians = sprite->rotation_degrees * Math_PI / 180.0f;

        if (sprite->face_motion) {
            draw_set_transform(shot->position, shot->get_rotation() + rotation_radians, Vector2(1, 1));
        } else {
            draw_set_transform(shot->position, rotation_radians, Vector2(1, 1));
        }

        draw_texture_rect_region(sprite->texture, Rect2(sprite->region.size * -0.5f, sprite->region.size), sprite->region);
    }
}

Danmaku* Pattern::get_danmaku() {
    return danmaku;
}

ShotEffect* Pattern::make_effect(int p_id) {
    ERR_FAIL_INDEX_V(p_id, MAX_EFFECTS, nullptr);

    has_effects = true;

    if (effects[p_id] != nullptr) {
        effects[p_id]->free();
    }
    effects[p_id] = ShotEffect::_new();
    return effects[p_id];
}

void Pattern::single(Dictionary p_override) {
    pattern(1, p_override, [](Shot* p_shot) {});
}

void Pattern::layered(int p_layers, float p_min, float p_max, Dictionary p_override) {
    float step = (p_max - p_min) / (p_layers - 1);

    pattern(p_layers, p_override, [=](Shot* p_shot) {
        p_shot->speed = p_min + step * p_shot->local_id;
    });
}

void Pattern::circle(int p_count, Dictionary p_override) {
    pattern(p_count, p_override, [=](Shot* p_shot) {
        p_shot->direction = p_shot->direction.rotated(p_shot->local_id * (Math_TAU / (float)p_count));
    });
}

void Pattern::layered_circle(int p_count, int p_layers, float p_min, float p_max, Dictionary p_override) {
    float step = (p_max - p_min) / (p_layers - 1);

    pattern(p_count * p_layers, p_override, [=](Shot* p_shot) {
        int col = p_shot->local_id % p_count;
        int row = p_shot->local_id / p_count;
        p_shot->direction = p_shot->direction.rotated(col * (Math_TAU / (float)p_count));
        p_shot->speed = p_min + step * row;
    });
}

void Pattern::fan(int p_count, float p_theta, Dictionary p_override) {
    float base = -p_theta * 0.5f;
    float step = p_theta / (p_count - 1);

    pattern(p_count, p_override, [=](Shot* p_shot) {
        p_shot->direction = p_shot->direction.rotated(base + step * p_shot->local_id);
    });
}

void Pattern::layered_fan(int p_count, float p_theta, int p_layers, float p_min, float p_max, Dictionary p_override) {
    float a_base = -p_theta * 0.5f;
    float a_step = p_theta / (p_count - 1);
    float s_step = (p_max - p_min) / (p_layers - 1);

    pattern(p_count * p_layers, p_override, [=](Shot* p_shot) {
        int col = p_shot->local_id % p_count;
        int row = p_shot->local_id / p_count;
        p_shot->direction = p_shot->direction.rotated(a_base + a_step * col);
        p_shot->speed = p_min + s_step * row;
    });
}

void Pattern::custom(int p_count, String p_name, Dictionary p_override) {
    if (delegate == nullptr) {
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

void Pattern::_register_methods() {
    register_property<Pattern, Ref<Reference>>("delegate", &Pattern::delegate, nullptr);
    register_property<Pattern, Dictionary>("parameters", &Pattern::parameters, Dictionary());
    
    register_method("_enter_tree", &Pattern::_enter_tree);
    register_method("_exit_tree", &Pattern::_exit_tree);
    register_method("_physics_process", &Pattern::_physics_process);
    register_method("_draw", &Pattern::_draw);

    register_method("get_danmaku", &Pattern::get_danmaku);

    register_method("make_effect", &Pattern::make_effect);

    register_method("single", &Pattern::single);
    register_method("layered", &Pattern::layered);
    register_method("circle", &Pattern::circle);
    register_method("layered_circle", &Pattern::layered_circle);
    register_method("fan", &Pattern::fan);
    register_method("layered_fan", &Pattern::layered_fan);
    register_method("custom", &Pattern::custom);
}

void Pattern::_init() {
    danmaku = nullptr;
    delegate = nullptr;
    parameters = Dictionary();
    has_effects = false;
    for (int i = 0; i != MAX_EFFECTS; ++i) {
        effects[i] = nullptr;
    }
}

Pattern::~Pattern() {
    for (int i = 0; i != MAX_EFFECTS; ++i) {
        if (effects[i] != nullptr) {
            effects[i]->free();
        }
    }
}