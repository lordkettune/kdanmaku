#ifndef __KD_SELECTOR_HPP__
#define __KD_SELECTOR_HPP__

#include <Godot.hpp>

#include <utility>
#include <tuple>

namespace godot {

class Shot;

class ISelector {
    public:
        virtual ~ISelector() {};
        virtual bool select(Shot* shot) = 0;
};

template<typename... Args>
class Selector : public ISelector {
    private:
        bool(*_function)(Shot*, Args...);
        std::tuple<Args...> _args;

        template<std::size_t... I>
        bool call(Shot* shot, std::index_sequence<I...>)
        {
            return _function(shot, std::get<I>(_args)...);
        }
    
    public:
        Selector(bool(*function)(Shot*, Args...), Args&&... args)
            : _function(function), _args(args...)
        {}

        bool select(Shot* shot) override
        {
            return call(shot, std::index_sequence_for<Args...>{});
        }
};

}

#endif