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
    delete _singleton;
    _singleton = nullptr;
}

ISelector* Parser::parse_selector(String src)
{
    PoolStringArray split = src.split(":");
    
    String key = split[0];
    if (key.empty()) {
        Godot::print_error("Parsing failed, expected key on left side of ':'", "parse_selector", __FILE__, __LINE__);
        return nullptr;
    }

    String args_src = split[1];
    if (args_src.empty()) {
        Godot::print_error("Parsing failed, expected arguments on right side of ':'", "parse_selector", __FILE__, __LINE__);
        return nullptr;
    }

    PoolStringArray args = args_src.split(",");

    if (_selectors.count(key) == 0) {
        return nullptr;
    } else {
        return _selectors[key]->parse(args);
    }
}