#include "standard_lib.hpp"
#include "parser.hpp"
#include "shot.hpp"

using namespace godot;


// ======== ======== ======== ======== ======== ======== ======== ========
// Built-in selectors
// ======== ======== ======== ======== ======== ======== ======== ========

bool s_all(Shot* shot)
{
    return true;
}

bool s_time(Shot* shot, int time)
{
    return shot->time == time;
}

bool s_time_range(Shot* shot, int start, int end)
{
    return shot->time >= start && shot->time <= end;
}


// ======== ======== ======== ======== ======== ======== ======== ========
// Built-in actions
// ======== ======== ======== ======== ======== ======== ======== ========

void a_set_speed(Shot* shot, float speed)
{
    shot->speed = speed;
}

void a_accelerate(Shot* shot, float amount)
{
    shot->speed += amount;
}


void godot::register_standard_lib()
{
    Parser* parser = Parser::get_singleton();

    parser->register_selector(s_all, "all");
    parser->register_selector(s_time, "time");
    parser->register_selector(s_time_range, "time_range");

    parser->register_action(a_set_speed, "set_speed");
    parser->register_action(a_accelerate, "accelerate");
}