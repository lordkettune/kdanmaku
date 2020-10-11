#ifndef __KD_PARAMETERS_HPP__
#define __KD_PARAMETERS_HPP__

#include <Godot.hpp>

#include <utility>

namespace godot {

template<std::size_t I, typename T>
class _Param {
private:
    T value;

protected:
    T get()
    {
        return value; // TODO: pattern params
    }
};

template<std::size_t I, typename T, typename... Args>
class _Recurse : _Param<I, T>, _Recurse<I + 1, Args...>
{
};

template<typename... Args>
class Parameters : _Recurse<0, Args...> {
public:
    template<std::size_t I, typename T>
    inline T get()
    {
        return _Param<I, T>::get();
    }
};

}

#endif