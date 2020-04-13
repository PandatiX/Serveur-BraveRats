#ifndef SERVEURBRAVERATS_MOVE_HPP
#define SERVEURBRAVERATS_MOVE_HPP


#include <chrono>

class Move {
private:
    std::chrono::time_point<std::chrono::system_clock> datetime;
    int mvP1, mvP2;

public:
    Move(int, int);

    std::chrono::time_point<std::chrono::system_clock> getDatetime();
    int getMovePlayer1();
    int getMovePlayer2();

};


#endif //SERVEURBRAVERATS_MOVE_HPP