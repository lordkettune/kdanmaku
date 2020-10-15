#ifndef __KD_UTILS_HPP__
#define __KD_UTILS_HPP__

#include <Godot.hpp>

#include <unordered_map>
#include <vector>

namespace std {

template<>
struct hash<godot::String>
{
    std::size_t operator()(const godot::String& string) const
    {
        return string.hash();
    }
};

}

namespace godot {

template<typename T>
using Map = std::unordered_map<String, T>;

template<typename T>
using Vector = std::vector<T>;

}

#endif