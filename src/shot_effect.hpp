#ifndef __KD_SHOT_EFFECT_HPP__
#define __KD_SHOT_EFFECT_HPP__

// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// *:･ﾟ✧ shot_effect.hpp *:･ﾟ✧
// 
// A ShotEffect is an object containing a list of Commands that execute on a Shot each frame.
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

#include <Godot.hpp>

#include <utility>
#include <tuple>
#include <stdint.h>

#include "utils.hpp"

#define STATUS_CONTINUE 0
#define STATUS_EXIT 1

namespace godot {

class Shot;

// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// Command object
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

class ICommand {
public:
    virtual ~ICommand() {}
    virtual int execute(Shot* p_shot) = 0;
};

template<typename... Args>
class Command : public ICommand {
private:
    int(*function)(Shot*, Args...);
    std::tuple<Args...> args;

    template<std::size_t... Indices>
    inline int call(Shot* p_shot, std::index_sequence<Indices...>) {
        return function(p_shot, std::get<Indices>(args)...);
    }

public:
    Command(int(*p_function)(Shot*, Args...), Args... p_args)
        : function(p_function), args(p_args...)
    {}

    int execute(Shot* p_shot) override {
        return call(p_shot, std::index_sequence_for<Args...>{});
    }
};

// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// ShotEffect object
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

class ShotEffect : public Object {
    GODOT_CLASS(ShotEffect, Object)
public:
    void execute(Shot* p_shot);

    template <auto Fn, typename... Args>
    ShotEffect* push_command(Args... p_args) {
        commands.push_back(new Command<Args...>(Fn, p_args...));
        return this;
    }

    static uint32_t bitmask(Array p_effects);

    static void _register_methods();
    void _init() {}
    ~ShotEffect();

private:
    Vector<ICommand*> commands;
};

}

#endif