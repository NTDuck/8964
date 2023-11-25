#include <game.hpp>
#include <auxiliaries/globals.hpp>


int main(int argc, char* args[]) {
    auto game = Game::instantiate(globals::config::kGameFlags, globals::config::kWindowDimension, globals::config::kFrameRate, "8964");
    game->start();

    return 0;
}