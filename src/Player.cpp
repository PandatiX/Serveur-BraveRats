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

int Player::getSockfd() const {
    return sockfd;
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

        } else {

            std::basic_string<char> keyword = buffer.substr(0, 4);

            /***** SHUT *****/
            if (keyword == "SHUT") {

                gameManager->stop();

            }
            /***** PING *****/
            else if (keyword == "PING") {

                stream << "PONG";

            }
            /***** STOP *****/
            else if (keyword == "STOP") {

                if (_game != nullptr) {

                    if (_game->getCreator() == _this)
                        gameManager->endGame(_this);
                    else
                        _game->leave(_game->getPlayer2());

                }
                stream << "STOP";
                running = false;

            }
            /***** STRT *****/
            else if (keyword == "STRT") {

                if (!(buffer.size() < 6 || (_game != nullptr && _game->getCreator() != _this))) {
                    //RE-
                    if (_game != nullptr) {
                        gameManager->endGame(_this);
                        stream << "REVD 402";
                    }
                    //START
                    *__game = _game = gameManager->createGame(_this);
                    if (*(buffer.substr(5, 1).c_str()) == '1')
                        _game->openMultiplayer();

                    if (stream.str().empty())
                        stream << "REVD 404";

                }

            }
            /***** GMLS *****/
            else if (keyword == "GMLS") {

                nlohmann::json j, arr = nlohmann::json::array();
                auto games = gameManager->getGames();
                for (Game *g : games) {
                    if (g->isJoinable() && g->getCreator() != _this)
                        arr.push_back(g->getCreator()->getSockaddrIn().sin_port);
                }
                j["players"] = arr;

                stream << "GMLS " << j.dump();

            }
            /***** JOIN *****/
            else if (keyword == "JOIN") {

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

            }
            /***** LEAV *****/
            else if (keyword == "LEAV") {

                if (_game != nullptr) {

                    if (_game->getCreator() == _this)
                        gameManager->endGame(_this);
                    else
                        _game->leave(_game->getPlayer2());
                    stream << "REVD 401";

                }

            }
            /***** PRMS *****/
            else if (keyword == "PRMS") {

                if (_game != nullptr && !_game->hasBegun()) {

                    if (buffer.size() > 10) {

                        std::basic_string<char> prmsKey = buffer.substr(5, 4);

                        if (prmsKey == "GVAR") {
                            int varCode = std::stoi(buffer.substr(10));
                            switch (varCode) {
                                case VARIANTE_FASTRAT:
                                    _game->setVariante(new VarianteFastRat());
                                    break;
                                case VARIANTE_PRINCESS:
                                    _game->setVariante(new VariantePrincess());
                                    break;
                                case VARIANTE_RANDOM:
                                    _game->setVariante(new VarianteRandom());
                                    break;
                                case VARIANTE_TRAITOR:
                                    _game->setVariante(new VarianteTraitor());
                                    break;
                                default:
                                case VARIANTE_DEFAULT:
                                    _game->setVariante(new VarianteDefault());
                                    break;
                            }
                        }

                        stream << "REVD 200";

                    }

                }

            }
            /***** CRDD *****/
            else if (keyword == "CRDD") {

                if (_game != nullptr && !_game->hasBegun()) {
                    _game->startGame();

                    Player *player1 = _game->getCreator();
                    Player *player2 = _game->getPlayer2();

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

            }
            /***** CRDP *****/
            else if (keyword == "CRDP") {

                if (buffer.size() > 5 && !buffer.substr(5).empty()) {
                    int nCard = std::stoi(buffer.substr(5));

                    if (_game != nullptr
                        && _game->hasBegun()
                        && _game->canPlay(_this, nCard)
                        && _game->getScore(_game->getCreator()) < _game->getVariante()->getScoreMax()
                        && _game->getScore(_game->getPlayer2()) < _game->getVariante()->getScoreMax()) {

                        int won = _game->play(_this, nCard);
                        stream << "WONP " << won;

                        //If game has ended (someone won)
                        if (*__game == nullptr)
                            stream << " FINAL";

                    }
                }

            }
            /***** CRDA *****/
            else if (keyword == "CRDA") {

                if (_game != nullptr && _game->hasBegun() && _game->cardBefore(_this) == 2 &&
                    (_game->cardBefore(_game->getCreator()) == 2) != (_game->cardBefore(_game->getPlayer2()) == 2)) {

                    int cardOther = _game->spyCard(_this);
                    stream << "CRDA " << cardOther;

                }

            }
            /***** CRDH *****/
            else if (keyword == "CRDH") {

                if (_game != nullptr && _game->hasBegun()) {

                    nlohmann::json j, jTmp, arr = nlohmann::json::array();
                    auto historyList = _game->getHistory();

                    std::string code;
                    if (buffer.size() > 5)
                        code = buffer.substr(5, 1);

                    if (code.empty()) {

                        for (Move *m : historyList) {
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

                            for (Move *m : historyList) {
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

            }
            /***** PNTS *****/
            else if (keyword == "PNTS") {

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
        }

        if (gameManager->getRunning()) {
            if (stream.str().empty())
                stream << "REVD 403";
            stream << std::endl;
            sendMessage(sockfd, stream.str());
        }

    }

    std::cout << "Client disconnection " << inet_ntoa(their_addr.sin_addr) << ":" << ntohs(their_addr.sin_port) << std::endl;

    if (gameManager->getRunning()) {
        gameManager->endGame(_this);
        shutdown(sockfd, SHUT_RDWR);
        close(sockfd);
    }

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