#include "GameManager.hpp"

GameManager *GameManager::singleton = nullptr;

GameManager::GameManager() {
    games = std::list<Game*>();
    players = std::list<Player*>();
}

GameManager* GameManager::getInstance() {
    if (!singleton)
        singleton = new GameManager();
    return singleton;
}

void GameManager::stop() {
    running = false;
}

int GameManager::start() {

    int sockfd, new_fd;
    struct sockaddr_in my_addr{};
    struct sockaddr_in their_addr{};
    socklen_t sin_size;
    int yes = 1;

    char localhost[128 +1];
    struct hostent *host;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cerr << "[-] Error in Connecton: ";
        return EXIT_FAILURE;
    }
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(MYPORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(my_addr.sin_zero), 8);

    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
        std::cerr << "[-] Error in binding";
        return EXIT_FAILURE;
    }

    gethostname(localhost, 128);
    host = gethostbyname(localhost);
    std::cout << "[+] Bind to port " << inet_ntoa(*(struct in_addr *)*host->h_addr_list) << ":" << MYPORT << std::endl;

    if (listen(sockfd, BACKLOG) == -1) {
        std::cerr << "[-] Error in listening";
        return EXIT_FAILURE;
    }
    std::cout << "[+] Listening..." << std::endl;

    while (running) {

        sin_size = sizeof(struct sockaddr_in);
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            std::cerr << "[-] Error in accepting connection on socket: ";
            return EXIT_FAILURE;
        }

        std::cout << "Server: got connection from " << inet_ntoa(their_addr.sin_addr) << ":" << ntohs(their_addr.sin_port) << std::endl;

        std::cout << "Socket fd: " << new_fd << std::endl;
        auto *p = new Player(new_fd, their_addr);
        addPlayer(p);
        p->start();

    }

    auto itEndGame = games.end();
    for (Game* g : games) {
        endGame(g->getCreator());
    }
    auto itEndPlayer = players.end();
    for (Player* p : players) {
        removePlayer(p);
    }

    std::cout << "Fin..." << std::endl;
    close(sockfd);

    return EXIT_SUCCESS;

}

void GameManager::addPlayer(Player *player) {
    //Check for disconnected players
    for (auto it = players.begin(); it != players.end(); ++it) {
        std::cout << (*it)->getSockaddrIn().sin_port << std::endl;
        if ((*it)->joinable()) {
            (*it)->join();
            removePlayer((*it));
            it = players.begin();
        }
    }

    if (player != nullptr) {
        if (!isKnown(player)) {
            players.push_back(player);
            std::cout << "Player " << player->getSockaddrIn().sin_port << " added to the list" << std::endl;
        }
    }
}

void GameManager::removePlayer(Player *player) {
    if (player != nullptr) {
        players.remove(player);
        delete player;
    }
}

Game* GameManager::createGame(Player *player) {
    if (player != nullptr && isKnown(player) && getGame(player) == nullptr) {
        Game* g = new Game(player);
        games.push_back(g);
        std::cout << "Player " << player->getSockaddrIn().sin_port << " ajouté à un partie" << std::endl;
        return g;
    }
    return nullptr;
}

void GameManager::endGame(Player *player) {
    if (player != nullptr) {
        Game *g = nullptr;
        auto itEnd = games.end();
        for (auto it = games.begin(); (it != itEnd && g == nullptr); it++) {
            if ((*it)->getCreator() == player || ((*it)->isOpenMultiplayer() && (*it)->getPlayer2() == player))
                g = (*it);
        }
        if (g != nullptr) {
            g->endGame();
            games.remove(g);
        }
    }
}

bool GameManager::getRunning() {
    return running;
}

bool GameManager::isKnown(Player *player) {
    if (player != nullptr) {
        auto itEnd = players.end();
        struct sockaddr_in s1 = player->getSockaddrIn(), s2{};
        for (auto it = players.begin(); it != itEnd; it++) {
            s2 = (*it)->getSockaddrIn();
            if (s1.sin_port == s2.sin_port)
                return true;
        }
    }
    return false;
}

Player* GameManager::getPlayer(struct sockaddr_in s) const {
    Player * p = nullptr;
    auto itEnd = players.end();
    struct sockaddr_in s2{};
    for (auto it = players.begin(); it != itEnd; it++) {
        s2 = (*it)->getSockaddrIn();
        if (s.sin_port == s2.sin_port)
            p = (*it);
    }
    return p;
}

Game* GameManager::getGame(Player *player) const {
    if (player != nullptr) {
        auto itEnd = games.end();
        for (auto it = games.begin(); it != itEnd; it++) {
            std::cout << "Game " << (*it)->getCreator()->getSockaddrIn().sin_port << std::endl;
            if ((*it)->getCreator() == player || (*it)->getPlayer2() == player)
                return (*it);
        }
    }
    return nullptr;
}

std::list<Game*> GameManager::getGames() const {
    return games;
}

GameManager::~GameManager() {
    delete singleton;
    games.clear();
    players.clear();
}