#include "frames.h"

void Frames::_notification(int p_what) {
    if (p_what == NOTIFICATION_PHYSICS_PROCESS) {
        if (!stopped) {
            frames_left--;
            if (frames_left <= 0) {
                stop();
                emit_signal("timeout");
            }
        }
    }
}

Frames* Frames::start(int p_frames) {
    ERR_FAIL_COND_V(!is_inside_tree(), this);
    set_physics_process(true);
    frames_left = p_frames;
    stopped = false;
    return this;
}

void Frames::stop() {
    set_physics_process(false);
    frames_left = -1;
    stopped = true;
}

int Frames::get_frames_left() const {
    return frames_left;
}

bool Frames::is_stopped() const {
    return frames_left == -1;
}

void Frames::_bind_methods() {
    ClassDB::bind_method(D_METHOD("start"), &Frames::start);
    ClassDB::bind_method(D_METHOD("stop"), &Frames::stop);
    ClassDB::bind_method(D_METHOD("is_stopped"), &Frames::is_stopped);
    ClassDB::bind_method(D_METHOD("get_frames_left"), &Frames::get_frames_left);

    ADD_SIGNAL(MethodInfo("timeout"));
}

Frames::Frames() {
    stop();
}