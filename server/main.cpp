/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** main
*/

#include <iostream>

#include "GameServer.hpp"

int main()
{
    try {
        rtype::GameServer server(60.0f, 30);

        if (!server.start(8080)) {
            return 1;
        }

        server.run();

    } catch (const std::exception& e) {
        std::cerr << "[Error] Exception: " << e.what() << std::endl;
        return 84;
    }

    return 0;
}
