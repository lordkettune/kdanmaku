#include "shot_effect.h"
#include "shot.h"
#include "pattern.h"

#include "core/method_bind_ext.gen.inc"

// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// Standard commands
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

int c_at(Shot* p_shot, int p_time) {
    return p_shot->get_time() == p_time ? STATUS_CONTINUE : STATUS_EXIT;
}

int c_after(Shot* p_shot, int p_time) {
    return p_shot->get_time() > p_time ? STATUS_CONTINUE : STATUS_EXIT;
}

int c_before(Shot* p_shot, int p_time) {
    return p_shot->get_time() < p_time ? STATUS_CONTINUE : STATUS_EXIT;
}

int c_between(Shot* p_shot, int p_start, int p_end) {
    return p_shot->get_time() > p_start && p_shot->get_time() < p_end ? STATUS_CONTINUE : STATUS_EXIT;
}

int c_every(Shot* p_shot, int p_start, int p_interval) {
    if (p_shot->get_time() >= p_start) {
        return (p_shot->get_time() - p_start) % p_interval == 0 ? STATUS_CONTINUE : STATUS_EXIT;
    }
    return STATUS_EXIT;
}


int c_set_position(Shot* p_shot, Vector2 p_position) {
    p_shot->set_position(p_position);
    return STATUS_CONTINUE;
}

int c_set_speed(Shot* p_shot, float p_speed) {
    p_shot->set_speed(p_speed);
    return STATUS_CONTINUE;
}

int c_set_direction(Shot* p_shot, Vector2 p_direction) {
    p_shot->set_direction(p_direction);
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

int c_set_effects(Shot* p_shot, Vector<int> p_effects) {
    p_shot->set_effects(p_effects);
    return STATUS_CONTINUE;
}


int c_despawn(Shot* p_shot) {
    p_shot->unflag(Shot::FLAG_ACTIVE);
    return STATUS_EXIT;
}

int c_accelerate(Shot* p_shot, float p_amount) {
    p_shot->set_speed(p_shot->get_speed() + p_amount);
    return STATUS_CONTINUE;
}

int c_min_speed(Shot* p_shot, float p_min) {
    if (p_shot->get_speed() < p_min) {
        p_shot->set_speed(p_min);
    }
    return STATUS_CONTINUE;
}

int c_max_speed(Shot* p_shot, float p_max) {
    if (p_shot->get_speed() > p_max) {
        p_shot->set_speed(p_max);
    }
    return STATUS_CONTINUE;
}

int c_rotate(Shot* p_shot, float p_amount) {
    p_shot->set_direction(p_shot->get_direction().rotated(p_amount));
    return STATUS_CONTINUE;
}


inline void sub_init(Shot* p_shot, Dictionary& p_override) {
    p_override["__offset"] = p_shot->get_position();
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

uint32_t ShotEffect::bitmask(const Vector<int>& p_effects) {
    uint32_t effects = 0;
    for (int i = 0; i != p_effects.size(); ++i) {
        effects |= (1 << p_effects[i]);
    }
    return effects;
}

template<typename T, T Fn, typename... Args>
inline void bind_command(const char* p_name, int(*_)(Shot*, Args...)) {
    ClassDB::bind_method(D_METHOD(p_name),  &ShotEffect::push_command<decltype(&Fn), Fn, Args...>);
}

#define ADD_COMMAND(m_n, m_fn) bind_command<decltype(&m_fn), m_fn>(m_n, m_fn)

void ShotEffect::_bind_methods() {
    ADD_COMMAND("at", c_at);
    ADD_COMMAND("after", c_after);
    ADD_COMMAND("before", c_before);
    ADD_COMMAND("between", c_between);
    ADD_COMMAND("every", c_every);

    ADD_COMMAND("set_position", c_set_position);
    ADD_COMMAND("set_speed", c_set_speed);
    ADD_COMMAND("set_direction", c_set_direction);
    ADD_COMMAND("set_rotation", c_set_rotation);
    ADD_COMMAND("set_sprite", c_set_sprite);
    ADD_COMMAND("set_effects", c_set_effects);

    ADD_COMMAND("despawn", c_despawn);
    ADD_COMMAND("accelerate", c_accelerate);
    ADD_COMMAND("min_speed", c_min_speed);
    ADD_COMMAND("max_speed", c_max_speed);
    ADD_COMMAND("rotate", c_rotate);

    ADD_COMMAND("sub_single", c_sub_single);
    ADD_COMMAND("sub_circle", c_sub_circle);
    ADD_COMMAND("sub_fan", c_sub_fan);
    ADD_COMMAND("sub_layered", c_sub_layered);
    ADD_COMMAND("sub_layered_circle", c_sub_layered_circle);
    ADD_COMMAND("sub_layered_fan", c_sub_layered_fan);
    ADD_COMMAND("sub_custom", c_sub_custom);
}

ShotEffect::~ShotEffect() {
    for (int i = 0; i != commands.size(); ++i) {
        memdelete(commands[i]);
    }
}