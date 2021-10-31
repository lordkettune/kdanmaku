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
    REG_STATE
};

class Shot;
class Pattern;

typedef uint32_t Command;
typedef uint8_t Register;

class ShotEffect : public Resource {
    GDCLASS(ShotEffect, Resource);

    Shot* current_shot;
    Pattern* current_pattern;
    Variant* current_state;
    
    Vector<Command> commands;
    Vector<Variant> constants;
    Vector<Variant> states;

    Ref<ShotEffect> next_pass;

protected:
    static void _bind_methods();

public:
    int val(const Variant& p_value);

    int move(int p_from, int p_to);
    int vmove(const Variant& p_value, int p_to);
    
    int add(int p_lhs, int p_rhs, int p_to);
    int sub(int p_lhs, int p_rhs, int p_to);
    int mul(int p_lhs, int p_rhs, int p_to);
    int div(int p_lhs, int p_rhs, int p_to);
    int mod(int p_lhs, int p_rhs, int p_to);

    int equal(int p_lhs, int p_rhs, int p_jump);
    int less(int p_lhs, int p_rhs, int p_jump);
    int lesseq(int p_lhs, int p_rhs, int p_jump);
    int test(int p_test, int p_jump);

    void patch(int p_ins);

    int fire();
    int reset();

    int timer(int p_reg);
    int yield();
    int end();
    int clear();

    int sfx(int p_from);
    int vsfx(const Variant& p_value);

    int debug(int p_reg);

    Register state(const Variant& p_default);
    void initialize_states(Variant* p_registers) const;

    void set_next_pass(Ref<ShotEffect> p_next_pass);
    Ref<ShotEffect> get_next_pass() const;
    int get_pass_count() const;

    void execute(Shot* p_shot);

    ShotEffect();

private:
    void execute_tick(Shot* p_shot, int p_id, Variant* p_state);
    void execute(Shot* p_shot, int p_id, Variant* p_state);

    void set_register(Register p_reg, const Variant& p_value);
    Variant get_register(Register p_reg) const;
};

#endif