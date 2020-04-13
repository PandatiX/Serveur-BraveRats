#include "src/Game.hpp"

int main(int argc, char **argv) {

    srand(time(nullptr));

    GameManager *gameManager = GameManager::getInstance();
    return gameManager->start();

}