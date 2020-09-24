#ifndef __KD_DANMAKU_HPP__
#define __KD_DANMAKU_HPP__

#include <Godot.hpp>
#include <Node2D.hpp>

namespace godot {

struct Shot {
    bool active;
    Vector2 position;
    Vector2 local_position;
    Vector2 direction;
    float speed;
};

class Danmaku : public Node2D {
    GODOT_CLASS(Danmaku, Node2D)

private:
    Shot* _shots;
    int* _free_ids;
    int _max_shots;

    int _free_count;
    int _active_count;

    Rect2 region;
    int tolerance;

    int max_shots;
    
public:
    static void _register_methods();
    void _init() {};
    bool is_danmaku();

    Danmaku();
    ~Danmaku();

    void _enter_tree();
    void _exit_tree();

    void capture_ids(int* buf, int count);
    void release_ids(int* buf, int count);

    inline Shot* get_shot(int id) { return &_shots[id]; }

    inline Rect2 get_region() { return region; }
    inline int get_tolerance() { return tolerance; }

    int get_free_shot_count()   { return _free_count;   }
    int get_active_shot_count() { return _active_count; }
};

}

#endif