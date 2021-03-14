// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// *:･ﾟ✧ shot_effect.hpp *:･ﾟ✧
// 
// A ShotEffect is an object containing a list of Commands that execute on a Shot each frame.
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

#ifndef SHOT_EFFECT_H
#define SHOT_EFFECT_H

#include "core/object.h"

// TODO: No std :P
#include <utility>
#include <tuple>

#define STATUS_CONTINUE 0
#define STATUS_EXIT 1

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
    GDCLASS(ShotEffect, Object);

    Vector<ICommand*> commands;

protected:
    static void _bind_methods();

public:
    void execute(Shot* p_shot);

    template <auto Fn, typename... Args>
    ShotEffect* push_command(Args... p_args) {
        commands.push_back(memnew(Command<Args...>(Fn, p_args...)));
        return this;
    }

    static uint32_t bitmask(const Vector<int>& p_effects);

    ShotEffect();
    ~ShotEffect();
};

#endif