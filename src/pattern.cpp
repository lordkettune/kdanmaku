#include "pattern.hpp"

#include <Math.hpp>

#include "hitbox.hpp"
#include "parser.hpp"

using namespace godot;

void Pattern::_register_methods()
{
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

Pattern::Pattern()
{
    _danmaku = nullptr;
    _current_local_id = 0;
    _fire_sprite = -1;
    _fire_radius = 0;
    _fire_direction = Vector2(1, 0);

    delegate = nullptr;
    fire_offset = Vector2(0, 0);
    fire_speed = 0;
}

Pattern::~Pattern()
{

}

void Pattern::_enter_tree()
{
    Node* parent = get_parent();
    while ((bool)parent->call("is_danmaku") != true) {
        parent = parent->get_parent();
        if (parent == nullptr) {
            Godot::print_error("Pattern is not a descendent of a Danmaku node!", "_enter_tree", __FILE__, __LINE__);
            return;
        }
    }
    _danmaku = Object::cast_to<Danmaku>(parent);
    _danmaku->count_pattern();
    set_z_index(_danmaku->get_z_index());
}

void Pattern::_exit_tree()
{
    // Release all shot IDs when exiting tree
    for (Shot* shot : _shots) {
        _danmaku->release(shot);
    }
    _danmaku->decount_pattern();
    _shots.clear();
}

void Pattern::_physics_process(float delta)
{
    if (_danmaku == nullptr) return;

    Transform2D transform = get_global_transform();
    Rect2 region = _danmaku->region.grow(_danmaku->tolerance);

    Hitbox* hitbox = _danmaku->get_hitbox();
    Vector2 hitbox_pos = Vector2(0, 0);
    if (hitbox != nullptr) {
        hitbox_pos = hitbox->get_global_transform().get_origin();
    }

    bool clean = false;

    for (Shot* shot : _shots) {
        if (!shot->active) {
            clean = true;
            continue;
        }

        shot->position += shot->direction * shot->speed;
        shot->global_position = transform.xform(shot->position);

        if (hitbox != nullptr) {
            float distance = shot->global_position.distance_to(hitbox_pos);

            if (distance <= hitbox->collision_radius + shot->radius) {
                if (!shot->is_colliding) {
                    hitbox->hit(shot);
                    shot->is_colliding = true;
                }
            } else {
                shot->is_colliding = false;
            }

            if (distance <= hitbox->graze_radius + shot->radius) {
                if (!shot->is_grazing) {
                    hitbox->graze(shot);
                    shot->is_grazing = true;
                }
            } else {
                shot->is_grazing = false;
            }
        }

        ++shot->time;

        for (auto const& mapping : _mappings) {
            if (mapping.selector->select(shot)) {
                mapping.action->apply(shot);
            }
        }

        if (!region.has_point(shot->global_position) || _danmaku->should_clear(shot->global_position)) {
            shot->active = false;
            clean = true;
        }
    }

    // Shots left danmaku region, release their IDs
    if (clean) {
        for (int i = 0; i != _shots.size();) {
            if (!_shots[i]->active) {
                _danmaku->release(_shots[i]);
                _shots.erase(_shots.begin() + i);
            } else {
                i++;
            }
        }
    }

    update();
}

void Pattern::_draw()
{
    for (Shot* shot : _shots) {
        ShotSprite* sprite = _danmaku->get_sprite(shot->sprite_id);
        sprite->draw_to(this, shot->position);
    }
}


void Pattern::set_fire_angle(float angle)
{
    _fire_direction = Vector2(cos(angle), sin(angle));
}

float Pattern::get_fire_angle()
{
    return _fire_direction.angle();
}

void Pattern::set_fire_sprite(String sprite)
{
    if (_danmaku == nullptr) {
        return;
    }
    _fire_sprite = _danmaku->get_sprite_id(sprite);
    if (_fire_sprite != -1) {
        _fire_radius = _danmaku->get_sprite(_fire_sprite)->collider_radius;
    }
}

String Pattern::get_fire_sprite()
{
    if (_fire_sprite != -1 && _danmaku != nullptr) {
        return _danmaku->get_sprite(_fire_sprite)->key;
    }
    return "";
}

bool Pattern::validate_fire()
{
    if (_danmaku == nullptr) {
        Godot::print_error("Pattern is not a descendent of a Danmaku node!", "validate_fire", __FILE__, __LINE__);
        return false;
    }

    if (_fire_sprite == -1) {
        Godot::print_error("Cannot fire until fire_sprite is set!", "validate_fire", __FILE__, __LINE__);
        return false;
    }

    return true;
}

Shot* Pattern::next_shot()
{
    Shot* shot = _danmaku->capture();
    shot->owner = this;
    shot->local_id = _current_local_id++;
    shot->time = 0;
    shot->sprite_id = _fire_sprite;
    shot->direction = _fire_direction;
    shot->radius = _fire_radius;
    shot->speed = fire_speed;
    shot->position = fire_offset;
    shot->is_grazing = false;
    shot->is_colliding = false;
    shot->active = true;
    _shots.push_back(shot);
    return shot;
}

void Pattern::fire()
{
    if (validate_fire()) {
        next_shot();
    }
}

void Pattern::fire_layered(int layers, float min, float max)
{
    if (validate_fire()) {
        float step = (max - min) / layers;
        for (int i = 0; i != layers; ++i) {
            Shot* shot = next_shot();
            shot->speed = min + step * i;
        }
    }
}

void Pattern::fire_circle(int count)
{
    if (validate_fire()) {
        float base = get_fire_angle();
        for (int i = 0; i != count; ++i) {
            Shot* shot = next_shot();
            float angle = base + i * (Math_TAU / (float)count);
            shot->direction = Vector2(cos(angle), sin(angle));
        }
    }
}

void Pattern::fire_layered_circle(int count, int layers, float min, float max)
{
    if (validate_fire()) {
        float base = get_fire_angle();
        float step = (min - max) / layers;

        for (int i = 0; i != count; ++i) {
            float angle = base + i * (Math_TAU / (float)count);
            Vector2 direction = Vector2(cos(angle), sin(angle));

            for (int j = 0; j != layers; ++j) {
                Shot* shot = next_shot();
                shot->direction = direction;
                shot->speed = min + j * step;
            }
        }
    }
}

void Pattern::fire_fan(int count, float theta)
{
    if (validate_fire()) {
        float base = get_fire_angle() - theta * 0.5f;
        float step = theta / (count - 1);
        
        for (int i = 0; i != count; ++i) {
            Shot* shot = next_shot();
            float angle = base + i * step;
            shot->direction = Vector2(cos(angle), sin(angle));
        }
    }
}

void Pattern::fire_layered_fan(int count, float theta, int layers, float min, float max)
{
    if (validate_fire()) {
        float angle_base = get_fire_angle() - theta * 0.5f;
        float angle_step = theta / (count - 1);
        float speed_step = (max - min) / layers;

        for (int i = 0; i != count; ++i) {
            float angle = angle_base + i * angle_step;
            Vector2 direction = Vector2(cos(angle), sin(angle));

            for (int j = 0; j != count; ++j) {
                Shot* shot = next_shot();
                shot->direction = direction;
                shot->speed = min + j * speed_step;
            }
        }
    }
}

void Pattern::fire_custom(int count, String name)
{
    if (delegate == nullptr) {
        Godot::print_error("Pattern does not have a delegate, can't fire custom pattern!", "fire_custom", __FILE__, __LINE__);
        return;
    }

    if (!delegate->has_method(name)) {
        Godot::print_error("Delegate has no custom pattern by name \"" + name + "\"", "fire_custom", __FILE__, __LINE__);
        return;
    }

    if (validate_fire()) {
        for (int i = 0; i != count; ++i) {
            Shot* shot = next_shot();
            delegate->call(name, count, i, shot);
        }
    }
}


ISelector* Pattern::make_selector(String source)
{
    return Parser::get_singleton()->parse_selector(source);
}

IAction* Pattern::make_action(String source)
{
    return Parser::get_singleton()->parse_action(source);
}

Array Pattern::select(String source)
{
    Array result;
    ISelector* selector = make_selector(source);
    if (selector == nullptr) {
        return result;
    }

    for (Shot* shot : _shots) {
        if (selector->select(shot)) {
            result.push_back(shot);
        }
    }
    
    delete selector;
    return result;
}

void Pattern::apply(String source)
{
    IAction* action = make_action(source);
    if (action == nullptr) {
        return;
    }

    for (Shot* shot : _shots) {
        action->apply(shot);
    }

    delete action;
}

void Pattern::map(String selector_source, String action_source)
{
    ISelector* selector = make_selector(selector_source);
    IAction* action = make_action(action_source);
    if (selector == nullptr || action == nullptr) {
        return;
    }

    _mappings.push_back({selector, action});
}