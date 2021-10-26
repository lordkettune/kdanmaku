// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// *:･ﾟ✧ shot_effect.hpp *:･ﾟ✧
// 
// A ShotEffect is an object containing a list of Commands that execute on a Shot each frame.
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

#ifndef SHOT_EFFECT_H
#define SHOT_EFFECT_H

#include "core/resource.h"

#define STATUS_CONTINUE 0
#define STATUS_YIELD 1

#define MAX_SHOT_EFFECTS 8

enum {
    REG_VALUE,
    REG_PATTERN,
    REG_SHOT,
    REG_CONSTANT
};

class Shot;
class Pattern;

typedef uint32_t Command;
typedef uint8_t Register;

class ShotEffect : public Resource {
    GDCLASS(ShotEffect, Resource);

    Shot* shot;
    Pattern* pattern;
    
    Vector<Command> commands;
    Vector<Variant> constants;

protected:
    static void _bind_methods();

public:
    int val(const Variant& p_value);

    int move(int p_from, int p_to);
    
    int add(int p_lhs, int p_rhs, int p_to);
    int sub(int p_lhs, int p_rhs, int p_to);
    int mul(int p_lhs, int p_rhs, int p_to);
    int div(int p_lhs, int p_rhs, int p_to);
    int mod(int p_lhs, int p_rhs, int p_to);

    int equal(int p_lhs, int p_rhs, int p_to);
    int less(int p_lhs, int p_rhs, int p_to);
    int lesseq(int p_lhs, int p_rhs, int p_to);
    int unot(int p_test, int p_to);

    int test(int p_test, int p_jump);
    void patch(int p_ins);

    int fire();
    int reset();

    /* DEPRECATED */
    int jumpif(int p_test, int p_jump);
    int jumpnot(int p_test, int p_jump);
    void jumphere(int p_ins);
    /* DEPRECATED */

    int timer(int p_reg);
    int yield();
    int end();
    int clear();

    int sfx(int p_from);

    void execute(int p_self, Shot* p_shot);

    ShotEffect() {}

private:
    void set_register(Register p_reg, const Variant& p_value);
    Variant get_register(Register p_reg) const;
    Variant get_constant(Register p_const) const;
};

#endif