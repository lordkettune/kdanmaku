#include "frames.hpp"

using namespace godot;

void Frames::_physics_process(float p_delta) {
    if (!stopped) {
        frames_left--;
        if (frames_left <= 0) {
            stop();
            emit_signal("timeout");
        }
    }
}

int Frames::get_frames_left() {
    return frames_left;
}

Frames* Frames::start(int p_frames) {
    ERR_FAIL_COND_V(!is_inside_tree(), this);
    frames_left = p_frames;
    stopped = false;
    set_physics_process(true);
    return this;
}

void Frames::stop() {
    frames_left = -1;
    stopped = true;
    set_physics_process(false);
}

bool Frames::is_stopped() {
    return frames_left == -1;
}

void Frames::_register_methods() {
    register_method("_physics_process", &Frames::_physics_process);
    register_method("start", &Frames::start);
    register_method("stop", &Frames::stop);
    register_method("is_stopped", &Frames::is_stopped);

    register_signal<Frames>("timeout", Dictionary());

    register_property<Frames, int>("frames_left", nullptr, &Frames::get_frames_left, -1);
}

void Frames::_init() {
    frames_left = -1;
    stopped = true;
    set_physics_process(false);
}