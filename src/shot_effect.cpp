#include "shot_effect.hpp"

#include "shot.hpp"

using namespace godot;

// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// Standard commands
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

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

uint32_t ShotEffect::bitmask(Array p_effects) {
    uint32_t effects = 0;
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
    register_command<c_accelerate>("accelerate");
    register_command<c_min_speed>("min_speed");
    register_command<c_max_speed>("max_speed");
    register_command<c_rotate>("rotate");
}

ShotEffect::~ShotEffect() {
    for (ICommand* command : commands) {
        delete command;
    }
}