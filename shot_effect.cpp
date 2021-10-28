#include "shot_effect.h"
#include "shot.h"
#include "pattern.h"
#include "hitbox.h"

#include "core/method_bind_ext.gen.inc"

enum {
    CMD_MOVE,

    CMD_ADD,
    CMD_SUB,
    CMD_MUL,
    CMD_DIV,
    CMD_MOD,

    CMD_EQ,
    CMD_LT,
    CMD_LE,
    CMD_NOT,

    CMD_TEST,

    CMD_FIRE,
    CMD_RESET,

    CMD_TIMER,
    CMD_YIELD,
    CMD_END,
    CMD_CLEAR,
    CMD_SFX
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

#define CURRENT (commands.size() - 1)

void ShotEffect::set_register(Register p_reg, const Variant& p_value) {
    switch (REG_SRC(p_reg)) {
        case REG_SHOT:
            shot->set_register(p_reg, p_value);
            break;
        
        case REG_PATTERN:
            pattern->set_register(p_reg, p_value);
            break;

        default:
        case REG_VALUE:
            ERR_FAIL_MSG("Cannot set a value or constant register!");
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
        case REG_VALUE:
            return constants[REG_IDX(p_reg)];
    }
}

int ShotEffect::val(const Variant& p_value) {
    constants.push_back(p_value);
    return REG_VALUE | ((constants.size() - 1) << 2);
}

int ShotEffect::move(int p_from, int p_to) {
    commands.push_back(MAKE_CMD_AB(CMD_MOVE, p_from, p_to));
    return CURRENT;
}

int ShotEffect::vmove(const Variant& p_value, int p_to) {
    commands.push_back(MAKE_CMD_AB(CMD_MOVE, val(p_value), p_to));
    return CURRENT;
}

int ShotEffect::add(int p_lhs, int p_rhs, int p_to) {
    commands.push_back(MAKE_CMD_ABC(CMD_ADD, p_lhs, p_rhs, p_to));
    return CURRENT;
}

int ShotEffect::sub(int p_lhs, int p_rhs, int p_to) {
    commands.push_back(MAKE_CMD_ABC(CMD_SUB, p_lhs, p_rhs, p_to));
    return CURRENT;
}

int ShotEffect::mul(int p_lhs, int p_rhs, int p_to) {
    commands.push_back(MAKE_CMD_ABC(CMD_MUL, p_lhs, p_rhs, p_to));
    return CURRENT;
}

int ShotEffect::div(int p_lhs, int p_rhs, int p_to) {
    commands.push_back(MAKE_CMD_ABC(CMD_DIV, p_lhs, p_rhs, p_to));
    return CURRENT;
}

int ShotEffect::mod(int p_lhs, int p_rhs, int p_to) {
    commands.push_back(MAKE_CMD_ABC(CMD_MOD, p_lhs, p_rhs, p_to));
    return CURRENT;
}

int ShotEffect::equal(int p_lhs, int p_rhs, int p_to) {
    commands.push_back(MAKE_CMD_ABC(CMD_EQ, p_lhs, p_rhs, p_to));
    return CURRENT;
}

int ShotEffect::less(int p_lhs, int p_rhs, int p_to) {
    commands.push_back(MAKE_CMD_ABC(CMD_LT, p_lhs, p_rhs, p_to));
    return CURRENT;
}

int ShotEffect::lesseq(int p_lhs, int p_rhs, int p_to) {
    commands.push_back(MAKE_CMD_ABC(CMD_LE, p_lhs, p_rhs, p_to));
    return CURRENT;
}

int ShotEffect::unot(int p_test, int p_to) {
    commands.push_back(MAKE_CMD_AB(CMD_NOT, p_test, p_to));
    return CURRENT;
}

int ShotEffect::test(int p_test, int p_jump) {
    commands.push_back(MAKE_CMD_AB(CMD_TEST, p_test, p_jump));
    return CURRENT;
}

void ShotEffect::patch(int p_ins) {
    int cmd = CMD(commands[p_ins]);
    int a = ARG_A(commands[p_ins]);
    ERR_FAIL_COND(cmd != CMD_TEST);
    commands.write[p_ins] = MAKE_CMD_AB(cmd, a, commands.size());
}

int ShotEffect::fire() {
    commands.push_back(CMD_FIRE);
    return CURRENT;
}

int ShotEffect::reset() {
    commands.push_back(CMD_RESET);
    return CURRENT;
}


/* DEPRECATED */
int ShotEffect::jumpif(int p_test, int p_jump) {
    WARN_PRINT_ONCE("jumpnot is deprecated, use test instead!");
    return test(p_test, p_jump);
}

int ShotEffect::jumpnot(int p_test, int p_jump) {
    WARN_PRINT_ONCE("jumpnot is deprecated, use unot and test instead!");
    unot(p_test, p_test);
    return test(p_test, p_jump);
}

void ShotEffect::jumphere(int p_ins) {
    WARN_PRINT_ONCE("jumpif is deprecated, use patch instead!");
    patch(p_ins);
}
/* DEPRECATED */


int ShotEffect::timer(int p_reg) {
    commands.push_back(MAKE_CMD_A(CMD_TIMER, p_reg));
    return CURRENT;
}

int ShotEffect::yield() {
    commands.push_back(CMD_YIELD);
    return CURRENT;
}

int ShotEffect::end() {
    commands.push_back(CMD_END);
    return CURRENT;
}

int ShotEffect::clear() {
    commands.push_back(CMD_CLEAR);
    return CURRENT;
}

int ShotEffect::sfx(int p_from) {
    commands.push_back(MAKE_CMD_A(CMD_SFX, p_from));
    return CURRENT;
}

int ShotEffect::vsfx(const Variant& p_value) {
    commands.push_back(MAKE_CMD_A(CMD_SFX, val(p_value)));
    return CURRENT;
}

void ShotEffect::set_next_pass(Ref<ShotEffect> p_next_pass) {
    next_pass = p_next_pass;
}

Ref<ShotEffect> ShotEffect::get_next_pass() const {
    return next_pass;
}

int ShotEffect::get_pass_count() const {
    if (next_pass.is_valid()) {
        return 1 + next_pass->get_pass_count();
    }
    return 1;
}

void ShotEffect::execute_tick(Shot* p_shot, int p_id) {
    shot = p_shot;
    pattern = p_shot->get_pattern();

    int* ins = shot->get_instruction_pointer(p_id);
    if (*ins == -1) {
        return;
    }

    while (*ins < commands.size()) {
Begin:
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
            
            case CMD_EQ:
                set_register(ARG_C(cmd), Variant::evaluate(Variant::OP_EQUAL, get_register(ARG_A(cmd)), get_register(ARG_B(cmd))));
                break;
            
            case CMD_LT:
                set_register(ARG_C(cmd), Variant::evaluate(Variant::OP_LESS, get_register(ARG_A(cmd)), get_register(ARG_B(cmd))));
                break;
            
            case CMD_LE:
                set_register(ARG_C(cmd), Variant::evaluate(Variant::OP_LESS_EQUAL, get_register(ARG_A(cmd)), get_register(ARG_B(cmd))));
                break;
            
            case CMD_NOT:
                set_register(ARG_B(cmd), !get_register(ARG_A(cmd)));
                break;
                        
            case CMD_TEST:
                if (get_register(ARG_A(cmd))) {
                    ERR_FAIL_INDEX(ARG_B(cmd), commands.size());
                    *ins = ARG_B(cmd);
                    goto Begin;
                }
                break;
            
            case CMD_FIRE:
                shot->get_pattern()->fire();
                break;
            
            case CMD_RESET:
                shot->get_pattern()->reset();
                break;
            
            case CMD_TIMER:
                {
                    Variant timer = get_register(ARG_A(cmd));
                    if (Variant::evaluate(Variant::OP_GREATER, timer, 0)) {
                        set_register(ARG_A(cmd), Variant::evaluate(Variant::OP_SUBTRACT, timer, 1));
                        return;
                    }
                }
                break;
            
            case CMD_YIELD:
                *ins = (*ins + 1) % commands.size();
                return;
            
            case CMD_END:
                *ins = -1;
                return;
            
            case CMD_CLEAR:
                shot->clear();
                return;
            
            case CMD_SFX:
                shot->get_pattern()->play_sfx(get_register(ARG_A(cmd)));
                break;
        }

        *ins = *ins + 1;
    }

    *ins = *ins % commands.size();
}

void ShotEffect::execute(Shot* p_shot, int p_id) {
    execute_tick(p_shot, p_id);
    if (next_pass.is_valid()) {
        next_pass->execute(p_shot, p_id + 1);
    }
}

void ShotEffect::execute(Shot* p_shot) {
    execute(p_shot, 0);
}

void ShotEffect::_bind_methods() {
    ClassDB::bind_method(D_METHOD("val", "value"), &ShotEffect::val);

    ClassDB::bind_method(D_METHOD("move", "from", "to"), &ShotEffect::move);
    ClassDB::bind_method(D_METHOD("vmove", "value", "to"), &ShotEffect::vmove);
    
    ClassDB::bind_method(D_METHOD("add", "lhs", "rhs", "to"), &ShotEffect::add);
    ClassDB::bind_method(D_METHOD("sub", "lhs", "rhs", "to"), &ShotEffect::sub);
    ClassDB::bind_method(D_METHOD("mul", "lhs", "rhs", "to"), &ShotEffect::mul);
    ClassDB::bind_method(D_METHOD("div", "lhs", "rhs", "to"), &ShotEffect::div);
    ClassDB::bind_method(D_METHOD("mod", "lhs", "rhs", "to"), &ShotEffect::mod);

    ClassDB::bind_method(D_METHOD("equal", "lhs", "rhs", "to"), &ShotEffect::equal);
    ClassDB::bind_method(D_METHOD("less", "lhs", "rhs", "to"), &ShotEffect::less);
    ClassDB::bind_method(D_METHOD("lesseq", "lhs", "rhs", "to"), &ShotEffect::lesseq);
    ClassDB::bind_method(D_METHOD("unot", "test", "to"), &ShotEffect::unot);

    ClassDB::bind_method(D_METHOD("test", "test", "jump"), &ShotEffect::test);
    ClassDB::bind_method(D_METHOD("patch", "ins"), &ShotEffect::patch);

    ClassDB::bind_method(D_METHOD("fire"), &ShotEffect::fire);
    ClassDB::bind_method(D_METHOD("reset"), &ShotEffect::reset);

    /* DEPRECATED */
    ClassDB::bind_method(D_METHOD("jumpif", "test", "jump"), &ShotEffect::jumpif);
    ClassDB::bind_method(D_METHOD("jumpnot", "test", "jump"), &ShotEffect::jumpnot);
    ClassDB::bind_method(D_METHOD("jumphere", "ins"), &ShotEffect::jumphere);
    /* DEPRECATED */

    ClassDB::bind_method(D_METHOD("timer", "reg"), &ShotEffect::timer);
    ClassDB::bind_method(D_METHOD("yield"), &ShotEffect::yield);
    ClassDB::bind_method(D_METHOD("end"), &ShotEffect::end);
    ClassDB::bind_method(D_METHOD("clear"), &ShotEffect::clear);

    ClassDB::bind_method(D_METHOD("sfx", "from"), &ShotEffect::sfx);
    ClassDB::bind_method(D_METHOD("vsfx", "value"), &ShotEffect::vsfx);

    ClassDB::bind_method(D_METHOD("set_next_pass", "next_pass"), &ShotEffect::set_next_pass);
    ClassDB::bind_method(D_METHOD("get_next_pass"), &ShotEffect::get_next_pass);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "next_pass", PROPERTY_HINT_RESOURCE_TYPE, "ShotEffect"), "set_next_pass", "get_next_pass");
}

ShotEffect::ShotEffect() {
    shot = nullptr;
    pattern = nullptr;

    commands = Vector<Command>();
    constants = Vector<Variant>();

    next_pass = Ref<ShotEffect>();
}