#ifndef __KD_FRAMES_HPP__
#define __KD_FRAMES_HPP__

// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// *:･ﾟ✧ frames.hpp *:･ﾟ✧
// 
// Replacement for Godot's built-in Timer object that operates in physics frames instead of seconds,
// to be consistent with the way time works in the rest of the plugin.
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

#include <Godot.hpp>
#include <Node.hpp>

namespace godot {

class Frames : public Node {
    GODOT_CLASS(Frames, Node);

    int frames_left;
    bool stopped;

public:
    int get_frames_left();

    Frames* start(int p_frames);
    void stop();
    bool is_stopped();

    void _physics_process(float p_delta);

    static void _register_methods();
    void _init();
};

};

#endif