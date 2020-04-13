#include "Move.hpp"

Move::Move(int _mvP1, int _mvP2) {
    datetime = std::chrono::system_clock::now();
    mvP1 = _mvP1;
    mvP2 = _mvP2;
}

std::chrono::time_point<std::chrono::system_clock> Move::getDatetime() {
    return datetime;
}

int Move::getMovePlayer1() {
    return mvP1;
}

int Move::getMovePlayer2() {
    return mvP2;
}