#ifndef SERVEURBRAVERATS_PLAYER_HPP
#define SERVEURBRAVERATS_PLAYER_HPP

#include "GameManager.hpp"

#include <thread>
#include <mutex>

class Game;
class GameManager;

class Player {
private:
    GameManager* gameManager;
    Game* game;
    int sockfd;
    struct sockaddr_in their_addr{};
    std::thread thread;
    bool threadJoinable;

    static void play(int, struct sockaddr_in, bool*, Game**, GameManager *);

public:
    Player(int, struct sockaddr_in);

    struct sockaddr_in getSockaddrIn();
    bool joinable();

    void start();
    void join();
    void leaveGame(Game *g);

    ~Player();

};

#include "Game.hpp"

#endif //SERVEURBRAVERATS_PLAYER_HPP