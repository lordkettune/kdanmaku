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
    virtual void apply(Shot* shot) = 0;
};

template<typename... Args>
class Action : public IAction {
private:
    void(*_function)(Shot*, Args...);
    std::tuple<Args...> _args;

    template<std::size_t... Indices>
    void call(Shot* shot, std::index_sequence<Indices...>)
    {
        return _function(shot, std::get<Indices>(_args)...);
    }

public:
    Action(void(*function)(Shot*, Args...), Args&&... args)
        : _function(function), _args(args...)
    {}

    void apply(Shot* shot) override
    {
        call(shot, std::index_sequence_for<Args...>{});
    }
};

}

#endif