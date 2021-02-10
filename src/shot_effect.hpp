#ifndef __KD_SHOT_EFFECT_HPP__
#define __KD_SHOT_EFFECT_HPP__

#include <Godot.hpp>

#include <utility>
#include <tuple>

#include "utils.hpp"

namespace godot {

class Shot;

class ICommand {
public:
    virtual int execute(Shot* p_shot) = 0;
};

template<typename... Args>
class Command : public ICommand {
private:
    void(*function)(Shot*, Args...);
    std::tuple<Args...> args;

    template<std::size_t... Indices>
    inline void call(Shot* p_shot, std::index_sequence<Indices...>) {
        function(p_shot, std::get<Indices>(args)...);
    }

public:
    Command(void(*p_function)(Shot*, Args...), Args&&... p_args)
        : function(p_function), args(p_args...)
    {}

    int execute(Shot* p_shot) override {
        call(p_shot, std::index_sequence_for<Args...>{});
        return 1;
    }
};

class ShotEffect : public Reference {
    GODOT_CLASS(ShotEffect, Reference);
public:
    String name;

    void execute(Shot* p_shot);

    static void _register_methods();
    void _init();

private:
    Vector<ICommand> commands;
};

};

#endif