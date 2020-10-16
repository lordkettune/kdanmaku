#include "parser.hpp"

using namespace godot;

Parser* Parser::_singleton = nullptr;

Parser* Parser::get_singleton()
{
    if (_singleton == nullptr) {
        _singleton = new Parser();
    }
    return _singleton;
}

void Parser::free_singleton()
{
    for (auto selector : _singleton->_selectors) {
        delete selector.second;
    }
    for (auto action : _singleton->_actions) {
        delete action.second;
    }
    delete _singleton;
    _singleton = nullptr;
}

ISelector* Parser::parse_selector(String src)
{
    PoolStringArray split = src.split(":");
    
    String key = split[0].strip_edges();
    if (key.empty()) {
        Godot::print_error("Parsing failed, expected key on left side of ':'", "parse_selector", __FILE__, __LINE__);
        return nullptr;
    }

    PoolStringArray args;

    if (split.size() > 1) {
        String args_src = split[1];
        if (args_src.empty()) {
            Godot::print_error("Parsing failed, expected arguments on right side of ':'", "parse_selector", __FILE__, __LINE__);
            return nullptr;
        }
        args = args_src.split(",");
    }

    if (_selectors.count(key) == 0) {
        return nullptr;
    } else {
        return _selectors[key]->parse(args);
    }
}

IAction* Parser::parse_action(String src)
{
    PoolStringArray split = src.split(":");
    
    String key = split[0].strip_edges();
    if (key.empty()) {
        Godot::print_error("Parsing failed, expected key on left side of ':'", "parse_action", __FILE__, __LINE__);
        return nullptr;
    }

    PoolStringArray args;

    if (split.size() > 1) {
        String args_src = split[1];
        if (args_src.empty()) {
            Godot::print_error("Parsing failed, expected arguments on right side of ':'", "parse_action", __FILE__, __LINE__);
            return nullptr;
        }
        args = args_src.split(",");
    }

    if (_actions.count(key) == 0) {
        return nullptr;
    } else {
        return _actions[key]->parse(args);
    }
}