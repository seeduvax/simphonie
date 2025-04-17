#include "sol/sol.hpp"

#include <iostream>

int main(int argc, char** argv) {
    int err = 0;
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::os, sol::lib::math,
                       sol::lib::table, sol::lib::debug);
    try {
        for (int i = 1; i < argc; i++) {
            lua.safe_script("print(\"plop\")");
            lua.safe_script_file(argv[i]);
        }
    }
    catch (const sol::error& e) {
        std::cerr << "Lua execution error: " << e.what() << std::endl;
        err = 1;
    }
    return err;
}
