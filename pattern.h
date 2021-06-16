// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// *:･ﾟ✧ pattern.hpp *:･ﾟ✧
// 
// Pattern is the main object for this library -- all shot firing functionality is here.
// Each Pattern keeps its own list of Shots that it owns, which it returns to its parent Danmaku
// node upon deletion or when shots are cleared.
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

#ifndef PATTERN_H
#define PATTERN_H

#include "scene/2d/node_2d.h"

#include "danmaku.h"
#include "shot.h"
#include "shot_effect.h"
#include "shot_sprite.h"

#define PATTERN_REGISTERS 8 + 4
#define PATTERN_REG(idx) (REG_PATTERN | (idx << 2))

class Pattern : public Node2D {
    GDCLASS(Pattern, Node2D);

    Danmaku* danmaku;
    Vector<Shot*> shots;
    Object* delegate;

    struct {
        int count;
        String shape;
        String sprite;
        Vector2 offset;
        Vector<int> effects;
        float rotation;
        float speed;
        bool paused;
        bool aim;
    } params;

    int effect_count;
    Ref<ShotEffect> effects[MAX_SHOT_EFFECTS];
    Variant registers[PATTERN_REGISTERS];

    float despawn_distance;
    bool autodelete;

protected:
    void _notification(int p_what);
    static void _bind_methods();

public:
    enum {
        REG0 = PATTERN_REG(0),
        REG1 = PATTERN_REG(1),
        REG2 = PATTERN_REG(2),
        REG3 = PATTERN_REG(3),
        REG4 = PATTERN_REG(4),
        REG5 = PATTERN_REG(5),
        REG6 = PATTERN_REG(6),
        REG7 = PATTERN_REG(7),

        SHAPE0 = PATTERN_REG(8),
        SHAPE1 = PATTERN_REG(9),
        SHAPE2 = PATTERN_REG(10),
        SHAPE3 = PATTERN_REG(11),

        COUNT    = PATTERN_REG(12),
        SHAPE    = PATTERN_REG(13),
        SPRITE   = PATTERN_REG(14),
        OFFSET   = PATTERN_REG(15),
        EFFECTS  = PATTERN_REG(16),
        ROTATION = PATTERN_REG(17),
        SPEED    = PATTERN_REG(18),
        PAUSED   = PATTERN_REG(19),
        AIM      = PATTERN_REG(20)
    };

    void set_register(Register p_reg, const Variant& p_value);
    Variant get_register(Register p_reg) const;

    int add_effect(const Ref<ShotEffect>& p_effect);

    void fire();
    void reset();

    void shape_single(Shot* p_shot);
    void shape_circle(Shot* p_shot);
    void shape_fan(Shot* p_shot);
    void shape_single_layered(Shot* p_shot);
    void shape_circle_layered(Shot* p_shot);
    void shape_fan_layered(Shot* p_shot);
    void shape_custom(Shot* p_shot);

    template <typename F>
    void clear(F p_constraint);

    Danmaku* get_danmaku() const;
    void remove_from_danmaku();

    void set_delegate(Object* p_delegate);
    Object* get_delegate() const;

    void set_despawn_distance(float p_despawn_distance);
    float get_despawn_distance() const;

    void set_autodelete(bool p_autodelete);
    bool get_autodelete() const;

    int fill_buffer(real_t*& buf);

    Pattern();

private:
    void _tick();
};

// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// Template method implementations
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

template <typename Fn>
void Pattern::clear(Fn p_constraint) {
    for (int i = 0; i != shots.size(); ++i) {
        if (p_constraint(shots[i])) {
            shots[i]->clear();
        }
    }
}

#endif