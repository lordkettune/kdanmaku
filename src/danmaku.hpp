#ifndef __KD_DANMAKU_HPP__
#define __KD_DANMAKU_HPP__

#include <Godot.hpp>
#include <Node2D.hpp>

namespace godot {

struct Shot {
    Vector2 position;
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

    int max_shots;
    
public:
    static void _register_methods();
    void _init() {};
    bool is_danmaku();

    Danmaku();
    ~Danmaku();

    void _enter_tree();
    void _exit_tree();

    void get_free_ids(int* buf, int count);

    inline Shot* get_shot(int id) { return &_shots[id]; }
};

}

#endif