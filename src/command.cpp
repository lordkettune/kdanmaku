#include "command.hpp"

#include "shot.hpp"

namespace godot {

int c_accelerate(Shot* p_shot, float p_amount) {
    p_shot->speed += p_amount;
    return STATUS_CONTINUE;
}

int c_rotate(Shot* p_shot, float p_amount) {
    p_shot->direction = p_shot->direction.rotated(p_amount);
    return STATUS_CONTINUE;
}

}