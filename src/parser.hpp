#ifndef __KD_PARSER_HPP__
#define __KD_PARSER_HPP__

// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========
// *:･ﾟ✧ parser.hpp *:･ﾟ✧
// 
// Parser singleton for selectors and actions.
//
// parse_selector takes a string and returns a selector,
// parse_action takes a string and returns an action.
// ======== ======== ======== ======== ======== ======== ======== ======== ======== ======== ========

#include <Godot.hpp>
 
#include <unordered_map>
#include <utility>
#include <cstdlib>

#include "selector.hpp"
#include "action.hpp"
#include "utils.hpp"

namespace godot {

class Shot;

// Parsing functions for individual data types. Done with template specialization.
template<typename T>
T parse_argument(String p_src) {
    return T();
}

template<>
inline int parse_argument<int>(String p_src) {
    return p_src.to_int();
}

template<>
inline float parse_argument<float>(String p_src) {
    return p_src.to_float();
}

class Parser {
private:
    static Parser* singleton;

    // Factory base
    template<typename T>
    class IFactory {
    public:
        virtual T* parse(PoolStringArray p_args) = 0;
    };

    // Factory object for selectors
    template<typename... Args>
    class SelectorFactory : public IFactory<ISelector> {
    private:
        bool(*function)(Shot*, Args...);

        template<std::size_t... Indices>
        ISelector* parse_arguments(PoolStringArray p_args, std::index_sequence<Indices...>) {
            return new Selector(function, parse_argument<Args>(p_args[Indices])...);
        }

    public:
        SelectorFactory(bool(*p_fn)(Shot*, Args...))
            : function(p_fn)
        {}

        ISelector* parse(PoolStringArray p_args) override {
            if (p_args.size() != sizeof...(Args)) {
                ERR_PRINT("Invalid number of parameters passed to selector!");
                return nullptr;
            }
            return parse_arguments(p_args, std::index_sequence_for<Args...>{});
        }
    };

    // Factory object for actions
    template<typename... Args>
    class ActionFactory : public IFactory<IAction> {
    private:
        void(*function)(Shot*, Args...);

        template<std::size_t... Indices>
        IAction* parse_arguments(PoolStringArray p_args, std::index_sequence<Indices...>) {
            return new Action(function, parse_argument<Args>(p_args[Indices])...);
        }

    public:
        ActionFactory(void(*p_fn)(Shot*, Args...))
            : function(p_fn)
        {}

        IAction* parse(PoolStringArray p_args) override {
            if (p_args.size() != sizeof...(Args)) {
                ERR_PRINT("Invalid number of parameters passed to action!");
                return nullptr;
            }
            return parse_arguments(p_args, std::index_sequence_for<Args...>{});
        }
    };

    Map<IFactory<ISelector>*> selectors;   // Map of selector keys to factory objects
    Map<IFactory<IAction>*> actions;       // Map of action keys to factory objects

public:
    static Parser* get_singleton();
    static void free_singleton();

    ISelector* parse_selector(String p_src);
    IAction* parse_action(String p_src);

    // Registers a native selector. See standard_lib.cpp
    template<typename... Args>
    void register_selector(bool(*p_fn)(Shot*, Args...), String p_key) {
        selectors[p_key] = new SelectorFactory<Args...>(p_fn);
    }

    // Registers a native action. See standard_lib.cpp
    template<typename... Args>
    void register_action(void(*p_fn)(Shot*, Args...), String p_key) {
        actions[p_key] = new ActionFactory<Args...>(p_fn);
    }
};

}

#endif