#ifndef __KD_SELECTOR_HPP__
#define __KD_SELECTOR_HPP__

// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// *:･ﾟ✧ selector.hpp *:･ﾟ✧
// 
// A Selector is a function that takes a shot, and returns a bool.
// They are used to filter shots to a subset that meets a list of conditions.
//
// They are created in two methods, Pattern.select and Pattern.map, via a string in the format:
//     selector_name:param1, param2, param3...
//
// This system is vaguely inspired by jQuery, in that a selector string is used to get a subset of
// items, then apply some change to that collection.
//
// In terms of implementation, this is basically just my own implemention of std::bind. I did it
// myself so I can expand it later with more operators.
//
// See pattern.hpp for how these are used, and parser.hpp for how they are created from a string.
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

#include <Godot.hpp>

#include <utility>
#include <tuple>

namespace godot {

class Shot;

class ISelector {
public:
    virtual ~ISelector() {};
    virtual bool select(Shot* p_shot) = 0;
};

template<typename... Args>
class Selector : public ISelector {
private:
    bool(*function)(Shot*, Args...);
    std::tuple<Args...> args;

    template<std::size_t... Indices>
    bool call(Shot* p_shot, std::index_sequence<Indices...>) {
        return function(p_shot, std::get<Indices>(args)...);
    }

public:
    Selector(bool(*p_function)(Shot*, Args...), Args&&... p_args)
        : function(p_function), args(p_args...)
    {}

    bool select(Shot* p_shot) override {
        return call(p_shot, std::index_sequence_for<Args...>{});
    }
};

}

#endif