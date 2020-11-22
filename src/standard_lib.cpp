#include "standard_lib.hpp"
#include "parser.hpp"
#include "shot.hpp"

using namespace godot;

// ======== ======== ======== ======== ======== ======== ======== ========
// Built-in selectors
// ======== ======== ======== ======== ======== ======== ======== ========

bool s_all(Shot* p_shot) {
    return true;
}

bool s_time(Shot* p_shot, int p_time) {
    return p_shot->time == p_time;
}

bool s_time_range(Shot* p_shot, int p_start, int p_end) {
    return p_shot->time >= p_start && p_shot->time <= p_end;
}

// ======== ======== ======== ======== ======== ======== ======== ========
// Built-in actions
// ======== ======== ======== ======== ======== ======== ======== ========

void a_set_speed(Shot* p_shot, float p_speed) {
    p_shot->speed = p_speed;
}

void a_change_speed(Shot* p_shot, float p_amount) {
    p_shot->speed += p_amount;
}


void godot::register_standard_lib() {
    Parser* parser = Parser::get_singleton();

    parser->register_selector(s_all, "all");
    parser->register_selector(s_time, "time");
    parser->register_selector(s_time_range, "time_range");

    parser->register_action(a_set_speed, "set_speed");
    parser->register_action(a_change_speed, "change_speed");
}