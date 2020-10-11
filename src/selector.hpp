#ifndef __KD_SELECTOR_HPP__
#define __KD_SELECTOR_HPP__

#include <Godot.hpp>

#include <utility>

#include "parameters.hpp"

namespace godot {

class Shot;

class ISelector {
    public:
        virtual ~ISelector() = 0;
        virtual bool select(Shot* shot) = 0;
};

template<typename... Args>
class Selector : public ISelector, Parameters<Args...> {
    private:
        bool(*function)(Shot*, Args...);

        template<std::size_t... I>
        bool call(Shot* shot, std::index_sequence<I...>)
        {
            return function(shot, get<I, Args>()...);
        }
    
    public:
        bool select(Shot* shot) override
        {
            return call(shot, std::index_sequence_for<Args...>{});
        }
};

}

#endif