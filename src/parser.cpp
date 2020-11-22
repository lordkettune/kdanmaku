#include "parser.hpp"

using namespace godot;

Parser* Parser::singleton = nullptr;

Parser* Parser::get_singleton() {
    if (singleton == nullptr) {
        singleton = new Parser();
    }
    return singleton;
}

void Parser::free_singleton() {
    for (auto selector : singleton->selectors) {
        delete selector.second;
    }
    for (auto action : singleton->actions) {
        delete action.second;
    }
    delete singleton;
    singleton = nullptr;
}

ISelector* Parser::parse_selector(String p_src) {
    PoolStringArray split = p_src.split(":");
    
    String key = split[0].strip_edges();
    if (key.empty()) {
        ERR_PRINT("Parsing failed, expected key on left side of ':'");
        return nullptr;
    }

    PoolStringArray args;

    if (split.size() > 1) {
        String args_src = split[1];
        if (args_src.empty()) {
            ERR_PRINT("Parsing failed, expected arguments on right side of ':'");
            return nullptr;
        }
        args = args_src.split(",");
    }

    if (selectors.count(key) == 0) {
        return nullptr;
    } else {
        return selectors[key]->parse(args);
    }
}

IAction* Parser::parse_action(String p_src) {
    PoolStringArray split = p_src.split(":");
    
    String key = split[0].strip_edges();
    if (key.empty()) {
        ERR_PRINT("Parsing failed, expected key on left side of ':'");
        return nullptr;
    }

    PoolStringArray args;

    if (split.size() > 1) {
        String args_src = split[1];
        if (args_src.empty()) {
            ERR_PRINT("Parsing failed, expected arguments on right side of ':'");
            return nullptr;
        }
        args = args_src.split(",");
    }

    if (actions.count(key) == 0) {
        return nullptr;
    } else {
        return actions[key]->parse(args);
    }
}