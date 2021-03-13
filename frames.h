// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// *:･ﾟ✧ frames.hpp *:･ﾟ✧
// 
// Replacement for Godot's built-in Timer object that operates in physics frames instead of seconds,
// to be consistent with the way time works in the rest of the plugin.
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

#ifndef FRAMES_H
#define FRAMES_H

#include "scene/main/node.h"

class Frames : public Node {
    GDCLASS(Frames, Node);

    int frames_left;
    bool stopped;

protected:
    void _notification(int p_what);
    static void _bind_methods();

public:
    int get_frames_left() const;

    Frames* start(int p_frames);
    void stop();
    bool is_stopped() const;

    Frames();
};

#endif