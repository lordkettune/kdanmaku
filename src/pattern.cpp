#include "pattern.hpp"

#include <Math.hpp>

#include "hitbox.hpp"
#include "parser.hpp"

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
    danmaku->count_pattern();
    set_z_index(danmaku->get_z_index());
}

void Pattern::_exit_tree() {
    // Release all shot IDs when exiting tree
    for (Shot* shot : shots) {
        danmaku->release(shot);
    }
    danmaku->decount_pattern();
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

    // If this is set while updating we need to clean the shot array
    bool clean = false;

    // Update all shots
    for (Shot* shot : shots) {
        if (!shot->active) {
            clean = true;
            continue;
        }

        // Move shot by its direction and speed, then update its global position
        shot->position += shot->direction * shot->speed;
        shot->global_position = transform.xform(shot->position);

        ++shot->time;

        // Check for graze or collision
        if (hitbox != nullptr) {
            float distance = shot->global_position.distance_to(hitbox_pos);

            if (distance <= hitbox->get_collision_radius() + shot->radius) {
                if (!shot->is_colliding) {
                    hitbox->hit(shot);
                    shot->is_colliding = true;
                }
            } else {
                shot->is_colliding = false;
            }

            if (distance <= hitbox->get_graze_radius() + shot->radius) {
                if (!shot->is_grazing) {
                    hitbox->graze(shot);
                    shot->is_grazing = true;
                }
            } else {
                shot->is_grazing = false;
            }
        }

        // Apply mappings -- test selector then apply action if it passes
        for (auto const& mapping : mappings) {
            if (mapping.selector->select(shot)) {
                mapping.action->apply(shot);
            }
        }

        // Clear shot if it's either outside the gameplay region or in clear circle
        if (!region.has_point(shot->global_position) || danmaku->should_clear(shot->global_position)) {
            shot->active = false;
            clean = true;
        }
    }

    // Shots left danmaku region, release them back to Danmaku
    if (clean) {
        for (int i = 0; i != shots.size();) {
            if (!shots[i]->active) {
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
        ShotSprite* sprite = danmaku->get_sprite(shot->sprite_id);
        sprite->draw_to(this, shot->position);
    }
}

Danmaku* Pattern::get_danmaku() {
    return danmaku;
}

void Pattern::set_fire_angle(float p_angle) {
    fire_direction = Vector2(cos(p_angle), sin(p_angle));
}

float Pattern::get_fire_angle() {
    return fire_direction.angle();
}

void Pattern::set_fire_sprite(String p_sprite) {
    ERR_FAIL_COND(danmaku == nullptr);
    fire_sprite = danmaku->get_sprite_id(p_sprite);
    if (fire_sprite != -1) {
        fire_radius = danmaku->get_sprite(fire_sprite)->collider_radius;
    }
}

String Pattern::get_fire_sprite() {
    if (fire_sprite != -1 && danmaku != nullptr) {
        return danmaku->get_sprite(fire_sprite)->key;
    }
    return "";
}

bool Pattern::validate_fire() {
    if (danmaku == nullptr) {
        ERR_PRINT("Pattern is not a descendent of a Danmaku node!");
        return false;
    }

    if (fire_sprite == -1) {
        ERR_PRINT("Cannot fire until fire_sprite is set!");
        return false;
    }

    return true;
}

Shot* Pattern::next_shot() {
    Shot* shot = danmaku->capture();
    shot->owner = this;
    shot->local_id = current_local_id++;
    shot->time = 0;
    shot->sprite_id = fire_sprite;
    shot->direction = fire_direction;
    shot->radius = fire_radius;
    shot->speed = fire_speed;
    shot->position = fire_offset;
    shot->is_grazing = false;
    shot->is_colliding = false;
    shot->active = true;
    shots.push_back(shot);
    return shot;
}

void Pattern::fire() {
    if (validate_fire()) {
        next_shot();
    }
}

void Pattern::fire_layered(int p_layers, float p_min, float p_max) {
    if (validate_fire()) {
        float step = (p_max - p_min) / p_layers;
        for (int i = 0; i != p_layers; ++i) {
            Shot* shot = next_shot();
            shot->speed = p_min + step * i;
        }
    }
}

void Pattern::fire_circle(int p_count) {
    if (validate_fire()) {
        float base = get_fire_angle();
        for (int i = 0; i != p_count; ++i) {
            Shot* shot = next_shot();
            float angle = base + i * (Math_TAU / (float)p_count);
            shot->direction = Vector2(cos(angle), sin(angle));
        }
    }
}

void Pattern::fire_layered_circle(int p_count, int p_layers, float p_min, float p_max) {
    if (validate_fire()) {
        float base = get_fire_angle();
        float step = (p_min - p_max) / p_layers;

        for (int i = 0; i != p_count; ++i) {
            float angle = base + i * (Math_TAU / (float)p_count);
            Vector2 direction = Vector2(cos(angle), sin(angle));

            for (int j = 0; j != p_layers; ++j) {
                Shot* shot = next_shot();
                shot->direction = direction;
                shot->speed = p_min + j * step;
            }
        }
    }
}

void Pattern::fire_fan(int p_count, float p_theta) {
    if (validate_fire()) {
        float base = get_fire_angle() - p_theta * 0.5f;
        float step = p_theta / (p_count - 1);
        
        for (int i = 0; i != p_count; ++i) {
            Shot* shot = next_shot();
            float angle = base + i * step;
            shot->direction = Vector2(cos(angle), sin(angle));
        }
    }
}

void Pattern::fire_layered_fan(int p_count, float p_theta, int p_layers, float p_min, float p_max) {
    if (validate_fire()) {
        float angle_base = get_fire_angle() - p_theta * 0.5f;
        float angle_step = p_theta / (p_count - 1);
        float speed_step = (p_max - p_min) / p_layers;

        for (int i = 0; i != p_count; ++i) {
            float angle = angle_base + i * angle_step;
            Vector2 direction = Vector2(cos(angle), sin(angle));

            for (int j = 0; j != p_count; ++j) {
                Shot* shot = next_shot();
                shot->direction = direction;
                shot->speed = p_min + j * speed_step;
            }
        }
    }
}

void Pattern::fire_custom(int p_count, String p_name) {
    if (delegate == nullptr) {
        ERR_PRINT("Pattern does not have a delegate, can't fire custom pattern!");
        return;
    }

    if (!delegate->has_method(p_name)) {
        ERR_PRINT("Delegate has no custom pattern by name \"" + p_name + "\"");
        return;
    }

    if (validate_fire()) {
        for (int i = 0; i != p_count; ++i) {
            Shot* shot = next_shot();
            delegate->call(p_name, p_count, i, shot);
        }
    }
}

ISelector* Pattern::make_selector(String p_source) {
    return Parser::get_singleton()->parse_selector(p_source);
}

IAction* Pattern::make_action(String p_source) {
    return Parser::get_singleton()->parse_action(p_source);
}

Array Pattern::select(String p_source) {
    Array result;
    ISelector* selector = make_selector(p_source);
    if (selector == nullptr) {
        return result;
    }

    for (Shot* shot : shots) {
        if (selector->select(shot)) {
            result.push_back(shot);
        }
    }
    
    delete selector;
    return result;
}

void Pattern::apply(String p_source) {
    IAction* action = make_action(p_source);
    if (action == nullptr) {
        return;
    }

    for (Shot* shot : shots) {
        action->apply(shot);
    }

    delete action;
}

void Pattern::map(String p_selector_source, String p_action_source) {
    ISelector* selector = make_selector(p_selector_source);
    IAction* action = make_action(p_action_source);
    if (selector == nullptr || action == nullptr) {
        return;
    }
    mappings.push_back({selector, action});
}

void Pattern::_register_methods() {
    register_property<Pattern, Ref<Reference>>("delegate", &Pattern::delegate, nullptr);

    register_property<Pattern, float>("fire_speed", &Pattern::fire_speed, 0);
    register_property<Pattern, Vector2>("fire_offset", &Pattern::fire_offset, Vector2(0, 0));
    register_property<Pattern, String>("fire_sprite", &Pattern::set_fire_sprite, &Pattern::get_fire_sprite, "");
    register_property<Pattern, float>("fire_angle", &Pattern::set_fire_angle, &Pattern::get_fire_angle, 0);
    
    register_method("_enter_tree", &Pattern::_enter_tree);
    register_method("_exit_tree", &Pattern::_exit_tree);
    register_method("_physics_process", &Pattern::_physics_process);
    register_method("_draw", &Pattern::_draw);

    register_method("get_danmaku", &Pattern::get_danmaku);

    register_method("fire", &Pattern::fire);
    register_method("fire_layered", &Pattern::fire_layered);
    register_method("fire_circle", &Pattern::fire_circle);
    register_method("fire_layered_circle", &Pattern::fire_layered_circle);
    register_method("fire_fan", &Pattern::fire_fan);
    register_method("fire_layered_fan", &Pattern::fire_layered_fan);
    register_method("fire_custom", &Pattern::fire_custom);

    register_method("select", &Pattern::select);
    register_method("apply", &Pattern::apply);
    register_method("map", &Pattern::map);
}

void Pattern::_init() {
    danmaku = nullptr;
    current_local_id = 0;
    fire_sprite = -1;
    fire_radius = 0;
    fire_direction = Vector2(1, 0);
    delegate = nullptr;
    fire_offset = Vector2(0, 0);
    fire_speed = 0;
}