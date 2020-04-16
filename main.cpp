#include "src/Game.hpp"

int main(int argc, char **argv) {

    GameManager *gameManager = GameManager::getInstance();
    return gameManager->start();

}