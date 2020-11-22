#ifndef __KD_ACTION_HPP__
#define __KD_ACTION_HPP__

// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// *:･ﾟ✧ action.hpp *:･ﾟ✧
// 
// An Action is simply a function that takes a shot and changes it somehow.
// See selector.hpp -- This is exactly like a Selector, except it doesn't return a bool.
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

#include <Godot.hpp>

#include <utility>
#include <tuple>

namespace godot {

class Shot;

class IAction {
public:
    virtual ~IAction() {}
    virtual void apply(Shot* p_shot) = 0;
};

template<typename... Args>
class Action : public IAction {
private:
    void(*function)(Shot*, Args...);
    std::tuple<Args...> args;

    template<std::size_t... Indices>
    void call(Shot* p_shot, std::index_sequence<Indices...>) {
        return function(p_shot, std::get<Indices>(args)...);
    }

public:
    Action(void(*p_function)(Shot*, Args...), Args&&... p_args)
        : function(p_function), args(p_args...)
    {}

    void apply(Shot* p_shot) override {
        call(p_shot, std::index_sequence_for<Args...>{});
    }
};

}

#endif