#include "shot_effect.h"
#include "shot.h"
#include "pattern.h"

// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// Standard commands
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

int c_at(Shot* p_shot, int p_time) {
    return p_shot->time == p_time ? STATUS_CONTINUE : STATUS_EXIT;
}

int c_after(Shot* p_shot, int p_time) {
    return p_shot->time > p_time ? STATUS_CONTINUE : STATUS_EXIT;
}

int c_before(Shot* p_shot, int p_time) {
    return p_shot->time < p_time ? STATUS_CONTINUE : STATUS_EXIT;
}

int c_between(Shot* p_shot, int p_start, int p_end) {
    return p_shot->time > p_start && p_shot->time < p_end ? STATUS_CONTINUE : STATUS_EXIT;
}

int c_every(Shot* p_shot, int p_start, int p_interval) {
    if (p_shot->time >= p_start) {
        return (p_shot->time - p_start) % p_interval == 0 ? STATUS_CONTINUE : STATUS_EXIT;
    }
    return STATUS_EXIT;
}


int c_set_position(Shot* p_shot, Vector2 p_position) {
    p_shot->position = p_position;
    return STATUS_CONTINUE;
}

int c_set_speed(Shot* p_shot, float p_speed) {
    p_shot->speed = p_speed;
    return STATUS_CONTINUE;
}

int c_set_direction(Shot* p_shot, Vector2 p_direction) {
    p_shot->direction = p_direction;
    return STATUS_CONTINUE;
}

int c_set_rotation(Shot* p_shot, float p_rotation) {
    p_shot->set_rotation(p_rotation);
    return STATUS_CONTINUE;
}

int c_set_sprite(Shot* p_shot, String p_sprite) {
    p_shot->set_sprite(p_sprite);
    return STATUS_CONTINUE;
}

int c_set_effects(Shot* p_shot, Array p_effects) {
    p_shot->set_effects(p_effects);
    return STATUS_CONTINUE;
}


int c_despawn(Shot* p_shot) {
    p_shot->unflag(Shot::FLAG_ACTIVE);
    return STATUS_EXIT;
}

int c_accelerate(Shot* p_shot, float p_amount) {
    p_shot->speed += p_amount;
    return STATUS_CONTINUE;
}

int c_min_speed(Shot* p_shot, float p_min) {
    if (p_shot->speed < p_min) {
        p_shot->speed = p_min;
    }
    return STATUS_CONTINUE;
}

int c_max_speed(Shot* p_shot, float p_max) {
    if (p_shot->speed > p_max) {
        p_shot->speed = p_max;
    }
    return STATUS_CONTINUE;
}

int c_rotate(Shot* p_shot, float p_amount) {
    p_shot->direction = p_shot->direction.rotated(p_amount);
    return STATUS_CONTINUE;
}


inline void sub_init(Shot* p_shot, Dictionary& p_override) {
    p_override["__offset"] = p_shot->position;
    p_override["__rotation"] = p_shot->get_rotation();
}

int c_sub_single(Shot* p_shot, Dictionary p_override) {
    sub_init(p_shot, p_override);
    p_shot->get_pattern()->single(p_override);
    return STATUS_CONTINUE;
}

int c_sub_circle(Shot* p_shot, int p_count, Dictionary p_override) {
    sub_init(p_shot, p_override);
    p_shot->get_pattern()->circle(p_count, p_override);
    return STATUS_CONTINUE;
}

int c_sub_fan(Shot* p_shot, int p_count, float p_theta, Dictionary p_override) {
    sub_init(p_shot, p_override);
    p_shot->get_pattern()->fan(p_count, p_theta, p_override);
    return STATUS_CONTINUE;
}

int c_sub_layered(Shot* p_shot, int p_layers, float p_min, float p_max, Dictionary p_override) {
    sub_init(p_shot, p_override);
    p_shot->get_pattern()->layered(p_layers, p_min, p_max, p_override);
    return STATUS_CONTINUE;
}

int c_sub_layered_circle(Shot* p_shot, int p_count, int p_layers, float p_min, float p_max, Dictionary p_override) {
    sub_init(p_shot, p_override);
    p_shot->get_pattern()->layered_circle(p_count, p_layers, p_min, p_max, p_override);
    return STATUS_CONTINUE;
}

int c_sub_layered_fan(Shot* p_shot, int p_count, float p_theta, int p_layers, float p_min, float p_max, Dictionary p_override) {
    sub_init(p_shot, p_override);
    p_shot->get_pattern()->layered_fan(p_count, p_theta, p_layers, p_min, p_max, p_override);
    return STATUS_CONTINUE;
}

int c_sub_custom(Shot* p_shot, int p_count, String p_name, Dictionary p_override) {
    sub_init(p_shot, p_override);
    p_shot->get_pattern()->custom(p_count, p_name, p_override);
    return STATUS_CONTINUE;
}

// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// ShotEffect object
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

void ShotEffect::execute(Shot* p_shot) {
    for (int i = 0; i != commands.size(); ++i) {
        int status = commands[i]->execute(p_shot);
        if (status == STATUS_EXIT) {
            return;
        }
    }
}

unsigned int ShotEffect::bitmask(Array p_effects) {
    unsigned int effects = 0;
    for (int i = 0; i != p_effects.size(); ++i) {
        if (p_effects[i].get_type() == Variant::INT) {
            effects |= (1 << (int)p_effects[i]);
        }
    }
    return effects;
}

template<auto Fn, typename... Args>
inline void command_proxy(const char* p_name, int(*_)(Shot*, Args...)) {
    register_method(p_name, &ShotEffect::push_command<Fn, Args...>);
}

template<auto Fn>
void register_command(const char* p_name) {
    command_proxy<Fn>(p_name, Fn);
}

void ShotEffect::_register_methods() {
    register_command<c_at>("at");
    register_command<c_after>("after");
    register_command<c_before>("before");
    register_command<c_between>("between");
    register_command<c_every>("every");

    register_command<c_set_position>("set_position");
    register_command<c_set_speed>("set_speed");
    register_command<c_set_direction>("set_direction");
    register_command<c_set_rotation>("set_rotation");
    register_command<c_set_sprite>("set_sprite");
    register_command<c_set_effects>("set_effects");

    register_command<c_despawn>("despawn");
    register_command<c_accelerate>("accelerate");
    register_command<c_min_speed>("min_speed");
    register_command<c_max_speed>("max_speed");
    register_command<c_rotate>("rotate");

    register_command<c_sub_single>("sub_single");
    register_command<c_sub_circle>("sub_circle");
    register_command<c_sub_fan>("sub_fan");
    register_command<c_sub_layered>("sub_layered");
    register_command<c_sub_layered_circle>("sub_layered_circle");
    register_command<c_sub_layered_fan>("sub_layered_fan");
    register_command<c_sub_custom>("sub_custom");
}

ShotEffect::~ShotEffect() {
    for (ICommand* command : commands) {
        delete command;
    }
}