/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** main - Prototype simple du jeu R-Type
*/

#include <iostream>

#include "src/Game.hpp"

int main() {
    try {
        std::cout << "=== R-Type Prototype ===" << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  ZQSD: Move player" << std::endl;
        std::cout << "  SPACE: Shoot" << std::endl;
        std::cout << "  ESC: Quit" << std::endl;
        std::cout << "========================" << std::endl;

        Game game;
        game.run();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Game exited." << std::endl;
    return 0;
}
