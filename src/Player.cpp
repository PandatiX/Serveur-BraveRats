#include "Player.hpp"

void sendMessage(int fd, const std::string& outputString) {
    //Make sure outputString isn't n*MAXDATASIZE (could crash client)
    std::string sMsg = outputString;
    if (sMsg.length() % MAXDATASIZE == 0)
        sMsg.append("");
    std::cout << "Server send: " << sMsg.c_str() << std::endl;
    send(fd, sMsg.c_str(), sMsg.length()+1, MSG_NOSIGNAL);
}

Player::Player(int _sockfd, struct sockaddr_in _their_addr) {
    sockfd = _sockfd;
    their_addr = _their_addr;
    threadJoinable = false;

    game = nullptr;

    gameManager = GameManager::getInstance();
}

struct sockaddr_in Player::getSockaddrIn() {
    return their_addr;
}

bool Player::joinable() const {
    return threadJoinable;
}

void Player::play(int sockfd, struct sockaddr_in their_addr, bool* threadJoinable, Game** __game, GameManager *gameManager) {

    int nbrecv;
    bool running = true;
    char tmpBuffer[MAXDATASIZE + 1];
    std::ostringstream stream;
    std::string buffer;
    Player* _this = gameManager->getPlayer(their_addr);
    Game* _game = *__game;

    std::cout << "thread socket(" << sockfd << ") init" << std::endl;

    while (running && gameManager->getRunning()) {

        buffer = std::string("");
        stream.str("");

        //TODO find a way to know exact socket packet size
        //Extract data from socket
        do {
            bzero(tmpBuffer, sizeof(tmpBuffer));
            nbrecv = recv(sockfd, tmpBuffer, MAXDATASIZE, 0);
            if (buffer.size() < buffer.max_size() - MAXDATASIZE)
                buffer.append(tmpBuffer);
        } while (nbrecv == MAXDATASIZE);
        std::cout << "Client sent: " << buffer << std::endl;

        //Treatment
        if (nbrecv <= 0) {

            std::cout << "Terminating play" << std::endl;
            running = false;

        } else if (!gameManager->getRunning()) {

            stream << "STOP";
            running = false;

        } else if (buffer == "PING") {

            stream << "PONG";

        } else if (buffer.substr(0, 4) == "STOP") {

            if (_game != nullptr) {

                if (_game->getCreator() == _this)
                    gameManager->endGame(_this);
                else
                    _game->leave(_game->getPlayer2());

            }
            stream << "STOP";
            running = false;

        } else if (buffer.substr(0, 4) == "STRT") {

            if (!(buffer.size() < 6 || (_game != nullptr && _game->getCreator() != _this))) {
                //RE-
                if (_game != nullptr) {
                    gameManager->endGame(_this);
                    stream << "REVD 402";
                }
                //START
                *__game = _game = gameManager->createGame(_this);
                if (*(buffer.substr(5,1).c_str()) == '1')
                    _game->openMultiplayer();

                if (stream.str().empty())
                    stream << "REVD 404";

            }

        } else if (buffer.substr(0, 4) == "GMLS") {

            nlohmann::json j, arr = nlohmann::json::array();
            auto games = gameManager->getGames();
            for (Game* g : games) {
                if (g->isJoinable() && g->getCreator() != _this)
                    arr.push_back(g->getCreator()->getSockaddrIn().sin_port);
            }
            j["players"] = arr;

            stream << "GMLS " << j.dump();

        } else if (buffer.substr(0, 4) == "JOIN") {

            if (_game == nullptr && buffer.length() > 5 && !buffer.substr(5).empty()) {

                std::cout << "Adding player to game" << std::endl;

                //Get corresponding game
                int nGame = std::stoi(buffer.substr(5));
                Game *g = nullptr;
                auto games = gameManager->getGames();
                bool gameExists = false;
                auto itEnd = games.end();
                for (auto it = games.begin(); it != itEnd && !gameExists; it++) {
                    if ((*it)->getCreator()->getSockaddrIn().sin_port == nGame) {
                        gameExists = true;
                        g = (*it);
                    }
                }

                //If game exists and is joinable, add player to it
                if (gameExists && g->isJoinable()) {
                    *__game = _game = g->setSecondPlayer(_this);
                    stream << "REVD 200";
                }

            }

        } else if (buffer.substr(0, 4) == "LEAV") {

            if (_game != nullptr) {

                if (_game->getCreator() == _this)
                    gameManager->endGame(_this);
                else
                    _game->leave(_game->getPlayer2());
                stream << "REVD 401";

            }

        } else if (buffer.substr(0, 4) == "CRDD") {

            if (_game != nullptr && !_game->hasBegun()) {
                _game->startGame();

                Player* player1 = _game->getCreator();
                Player* player2 = _game->getPlayer2();

                int *cardsP1 = _game->getCards(player1);
                int *cardsP2 = _game->getCards(player2);

                nlohmann::json jOut, jArrP1 = nlohmann::json::array(), jArrP2 = nlohmann::json::array();

                for (int i = 0; i < 8; i++) {
                    jArrP1.push_back(cardsP1[i]);
                    jArrP2.push_back(cardsP2[i]);
                }

                if (_this == player1) {
                    jOut["self"] = jArrP1;
                    jOut["other"] = jArrP2;
                } else {
                    jOut["self"] = jArrP2;
                    jOut["other"] = jArrP1;
                }

                stream << "CRDD " << jOut.dump();
            }

        } else if (buffer.substr(0, 4) == "CRDP") {

            if (buffer.size() > 5 && !buffer.substr(5).empty()) {
                int nCard = (int)strtol(buffer.substr(5).c_str(), NULL, 10);

                if (_game != nullptr && _game->hasBegun() && _game->canPlay(_this, nCard)) {

                    int won = _game->play(_this, nCard, _game->getVariante());
                    stream << "WONP " << won;

                }
            }

        } else if (buffer.substr(0, 4) == "CRDA") {

            if (_game != nullptr && _game->hasBegun() && _game->cardBefore(_this) == 2 && (_game->cardBefore(_game->getCreator()) == 2) != (_game->cardBefore(_game->getPlayer2()) == 2)) {

                int cardOther = _game->spyCard(_this);
                stream << "CRDA " << cardOther;

            }

        } else if (buffer.substr(0, 4) == "CRDH") {

            if (_game != nullptr && _game->hasBegun()) {

                nlohmann::json j, jTmp, arr = nlohmann::json::array();
                auto historyList = _game->getHistory();

                std::string code;
                if (buffer.size() > 5)
                    code = buffer.substr(5, 1);

                if (code.empty()) {

                    for (Move* m : historyList) {
                        jTmp["dt"] = std::chrono::system_clock::to_time_t(m->getDatetime());
                        jTmp["j1"] = m->getMovePlayer1();
                        jTmp["j2"] = m->getMovePlayer2();
                        arr.push_back(jTmp);
                    }
                    j["play"] = arr;

                    stream << "CRDH " << j.dump();

                } else if (code == "0") {

                    bool firstPlayer = _game->getCreator() == _this;
                    if (firstPlayer || _game->getPlayer2() == _this) {

                        for(Move* m : historyList) {
                            jTmp["dt"] = std::chrono::system_clock::to_time_t(m->getDatetime());
                            if (firstPlayer) {
                                jTmp["self"] = m->getMovePlayer1();
                                jTmp["other"] = m->getMovePlayer2();
                            } else {
                                jTmp["self"] = m->getMovePlayer2();
                                jTmp["other"] = m->getMovePlayer1();
                            }
                            arr.push_back(jTmp);
                        }
                        j["play"] = arr;

                        stream << "CRDH " << j.dump();

                    }

                } else if (code == "1") {

                    bool firstPlayer = _game->getCreator() == _this;
                    if (firstPlayer || _game->getPlayer2() == _this) {

                        Move *lastMove = *historyList.rbegin();
                        if (lastMove != nullptr) {
                            jTmp["dt"] = std::chrono::system_clock::to_time_t(lastMove->getDatetime());
                            if (firstPlayer) {
                                jTmp["self"] = lastMove->getMovePlayer1();
                                jTmp["other"] = lastMove->getMovePlayer2();
                            } else {
                                jTmp["self"] = lastMove->getMovePlayer2();
                                jTmp["other"] = lastMove->getMovePlayer1();
                            }
                            arr.push_back(jTmp);
                        }
                        j["play"] = arr;

                        stream << "CRDH " << j.dump();

                    }

                }

            }

        } else if (buffer.substr(0, 4) == "PNTS") {

            if (_game != nullptr && _game->hasBegun()) {

                int scoreP1 = _game->getScore(_game->getCreator());
                int scoreP2 = _game->getScore(_game->getPlayer2());

                stream << "PNTS ";
                if (_this == _game->getCreator()) {
                    stream << scoreP1 << " " << scoreP2;
                } else if (_this == _game->getPlayer2()) {
                    stream << scoreP2 << " " << scoreP1;
                }

            }

        } else {

            stream << "ERRO mot-clé non reconnu: " << buffer.c_str();

        }

        if (stream.str().empty())
            stream << "REVD 403";
        stream << std::endl;
        sendMessage(sockfd, stream.str());

    }

    gameManager->endGame(_this);

    std::cout << "Client disconnection " << inet_ntoa(their_addr.sin_addr) << ":" << ntohs(their_addr.sin_port) << std::endl;
    close(sockfd);

    *threadJoinable = true;

}

void Player::start() {
    threadJoinable = false;
    thread = std::thread(play, sockfd, their_addr, &threadJoinable, &game, gameManager);
}

void Player::join() {
    thread.join();
    std::cout << "Joined thread" << std::endl;
}

void Player::leaveGame(Game *g) {
    if (g != nullptr && g == game) {
        game = nullptr;
    }
}

Player::~Player() = default;