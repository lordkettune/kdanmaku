#include "shot_effect.h"
#include "shot.h"
#include "pattern.h"

#include "core/method_bind_ext.gen.inc"

enum {
    CMD_MOVE,
    CMD_ADD,
    CMD_SUB,
    CMD_MUL,
    CMD_DIV,
    CMD_MOD
};

#define REG_SRC(reg) (reg & 0x03)
#define REG_IDX(reg) (reg >> 2)

#define CMD(cmd) (cmd & 0xFF)
#define ARG_A(cmd) ((cmd >> 8) & 0xFF)
#define ARG_B(cmd) ((cmd >> 16) & 0xFF)
#define ARG_C(cmd) ((cmd >> 24) & 0xFF)

#define MAKE_CMD_A(cmd, a) ((cmd & 0xFF) | ((a & 0xFF) << 8))
#define MAKE_CMD_AB(cmd, a, b) ((cmd & 0xFF) | ((a & 0xFF) << 8) | ((b & 0xFF) << 16))
#define MAKE_CMD_ABC(cmd, a, b, c) ((cmd & 0xFF) | ((a & 0xFF) << 8) | ((b & 0xFF) << 16) | ((c & 0xFF) << 24))

void ShotEffect::set_register(Register p_reg, const Variant& p_value) {
    switch (REG_SRC(p_reg)) {
        case REG_SHOT:
            shot->set_register(p_reg, p_value);
            break;
        
        case REG_PATTERN:
            pattern->set_register(p_reg, p_value);
            break;

        default:
        case REG_CONSTANT:
            ERR_FAIL_MSG("Cannot set a constant register!");
            break;
    }
}

Variant ShotEffect::get_register(Register p_reg) const {
    switch (REG_SRC(p_reg)) {
        case REG_SHOT:
            return shot->get_register(p_reg);
        
        case REG_PATTERN:
            return pattern->get_register(p_reg);
        
        default:
        case REG_CONSTANT:
            return constants[REG_IDX(p_reg)];
    }
}

int ShotEffect::constant(const Variant& p_value) {
    constants.push_back(p_value);
    return REG_CONSTANT | ((constants.size() - 1) << 2);
}

void ShotEffect::move(int p_from, int p_to) {
    commands.push_back(MAKE_CMD_AB(CMD_MOVE, p_from, p_to));
}

void ShotEffect::add(int p_lhs, int p_rhs, int p_to) {
    commands.push_back(MAKE_CMD_ABC(CMD_ADD, p_lhs, p_rhs, p_to));
}

void ShotEffect::sub(int p_lhs, int p_rhs, int p_to) {
    commands.push_back(MAKE_CMD_ABC(CMD_SUB, p_lhs, p_rhs, p_to));
}

void ShotEffect::mul(int p_lhs, int p_rhs, int p_to) {
    commands.push_back(MAKE_CMD_ABC(CMD_MUL, p_lhs, p_rhs, p_to));
}

void ShotEffect::div(int p_lhs, int p_rhs, int p_to) {
    commands.push_back(MAKE_CMD_ABC(CMD_DIV, p_lhs, p_rhs, p_to));
}

void ShotEffect::mod(int p_lhs, int p_rhs, int p_to) {
    commands.push_back(MAKE_CMD_ABC(CMD_MOD, p_lhs, p_rhs, p_to));
}

void ShotEffect::execute(int p_self, Shot* p_shot) {
    shot = p_shot;
    pattern = p_shot->get_pattern();

    int* ins = shot->get_instruction_pointer(p_self);
    if (*ins == -1) {
        return;
    }

    while (*ins < commands.size()) {
        Command cmd = commands[*ins];

        switch (CMD(cmd)) {
            case CMD_MOVE:
                set_register(ARG_B(cmd), get_register(ARG_A(cmd)));
                break;
            
            case CMD_ADD:
                set_register(ARG_C(cmd), Variant::evaluate(Variant::OP_ADD, get_register(ARG_A(cmd)), get_register(ARG_B(cmd))));
                break;
            
            case CMD_SUB:
                set_register(ARG_C(cmd), Variant::evaluate(Variant::OP_SUBTRACT, get_register(ARG_A(cmd)), get_register(ARG_B(cmd))));
                break;
            
            case CMD_MUL:
                set_register(ARG_C(cmd), Variant::evaluate(Variant::OP_MULTIPLY, get_register(ARG_A(cmd)), get_register(ARG_B(cmd))));
                break;
            
            case CMD_DIV:
                set_register(ARG_C(cmd), Variant::evaluate(Variant::OP_DIVIDE, get_register(ARG_A(cmd)), get_register(ARG_B(cmd))));
                break;
            
            case CMD_MOD:
                set_register(ARG_C(cmd), Variant::evaluate(Variant::OP_MODULE, get_register(ARG_A(cmd)), get_register(ARG_B(cmd))));
                break;
        }

        *ins = *ins + 1;
    }

    *ins = *ins % commands.size();
}

void ShotEffect::_bind_methods() {
    ClassDB::bind_method(D_METHOD("constant", "value"), &ShotEffect::constant);

    ClassDB::bind_method(D_METHOD("move", "from", "to"), &ShotEffect::move);
    
    ClassDB::bind_method(D_METHOD("add", "lhs", "rhs", "to"), &ShotEffect::add);
    ClassDB::bind_method(D_METHOD("sub", "lhs", "rhs", "to"), &ShotEffect::sub);
    ClassDB::bind_method(D_METHOD("mul", "lhs", "rhs", "to"), &ShotEffect::mul);
    ClassDB::bind_method(D_METHOD("div", "lhs", "rhs", "to"), &ShotEffect::div);
    ClassDB::bind_method(D_METHOD("mod", "lhs", "rhs", "to"), &ShotEffect::mod);
}