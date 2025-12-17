#include "tui.hpp"
#include <iostream>
#include <exception>

int main(int argc, char* argv[]) {
    try {
        TUI tui;
        tui.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

