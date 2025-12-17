#include "tui.hpp"
#include <iostream>
#include <exception>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    
    try {
        TUI tui;
        tui.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "tbm: " << e.what() << std::endl;
        return 1;
    }
}

