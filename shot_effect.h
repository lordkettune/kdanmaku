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
    REG_CONSTANT,
    REG_PATTERN,
    REG_SHOT
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
    int constant(const Variant& p_value);

    void move(int p_from, int p_to);
    
    void add(int p_lhs, int p_rhs, int p_to);
    void sub(int p_lhs, int p_rhs, int p_to);
    void mul(int p_lhs, int p_rhs, int p_to);
    void div(int p_lhs, int p_rhs, int p_to);
    void mod(int p_lhs, int p_rhs, int p_to);

    void execute(int p_self, Shot* p_shot);

    ShotEffect() {}

private:
    void set_register(Register p_reg, const Variant& p_value);
    Variant get_register(Register p_reg) const;
};

#endif