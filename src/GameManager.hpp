#ifndef SERVEURBRAVERATS_GAMEMANAGER_HPP
#define SERVEURBRAVERATS_GAMEMANAGER_HPP

#define MYPORT 3490
#define BACKLOG 10
#define MAXDATASIZE 1024

#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <iostream>
#include <thread>
#include <sstream>
#include <nlohmann/json.hpp>
#include <list>

class Game;
class Player;

class GameManager {
private:
    static GameManager *singleton;
    std::list<Game*> games;
    std::list<Player*> players;
    bool running = true;

    GameManager();

public:
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

    static GameManager *getInstance();

    void stop();
    void endGame(Player *);
    void addPlayer(Player*);
    void removePlayer(Player*);
    int start();

    bool getRunning();
    bool isKnown(Player*);
    Player* getPlayer(struct sockaddr_in) const;
    Game* createGame(Player*);
    Game* getGame(Player*) const;
    std::list<Game*> getGames() const;

    ~GameManager();

};

#include "Player.hpp"

#endif //SERVEURBRAVERATS_GAMEMANAGER_HPP
