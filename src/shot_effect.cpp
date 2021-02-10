#include "shot_effect.hpp"

using namespace godot;

void ShotEffect::execute(Shot* p_shot) {
    int instruction = 0;
    while (instruction < commands.size()) {
        instruction += commands[instruction].execute(p_shot);
    }
}

void ShotEffect::_register_methods() {
    register_property<ShotEffect, String>("name", &ShotEffect::name, "");
}

void ShotEffect::_init() {
    name = "";
}