#ifndef __KD_COMMAND_HPP__
#define __KD_COMMAND_HPP__

// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// *:･ﾟ✧ command.hpp *:･ﾟ✧
// 
// A Command is a method called on a shot per frame.
// A Pattern may contain a list of Commands that should be executed on its shots.
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

#include <utility>
#include <tuple>

#define STATUS_CONTINUE 0
#define STATUS_EXIT 1

namespace godot {

class Shot;

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
// Standard commands
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

int c_accelerate(Shot* p_shot, float p_amount);
int c_rotate(Shot* p_shot, float p_amount);

}

#endif