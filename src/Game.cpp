#include "Game.hpp"

Game::Game(Player *_player1) {
    //Init as singleplayer
    player1 = _player1;
    player2 = nullptr;

    multiplayer = began = false;
    cardP1 = cardBeforeP1 = cardP2 = cardBeforeP2 = winner = -1;
    scoreP1 = scoreP2 = scoreBuff = 0;

    //Init synchronization barriers
    barrier = new BarrierGame(1);
    barrierWinner = new BarrierGame(1);
    barrierP1 = new BarrierCard();
    barrierP2 = new BarrierCard();

    //Init gameplay
    variante = new VarianteDefault();
    variante->init(cardsPlayer1, cardsPlayer2);

    gameManager = GameManager::getInstance();
}

Game* Game::setSecondPlayer(Player *_player2) {
    if ((_player2 != nullptr && _player2 != player1 && gameManager->isKnown(_player2))
        || (_player2 == nullptr)) {
        player2 = _player2;
    }
    return this;
}

void Game::leave(Player *player) {
    if (player != nullptr) {
        //If creator
        if (player == player1) {
            endGame();
        }
        //If second player
        else if (player == player2) {
            player2->leaveGame(this);
            player2 = nullptr;
        }
    }
}

void Game::endGame() {
    if (player2 != nullptr) {
        player2->leaveGame(this);
        player2 = nullptr;
    }
    player1->leaveGame(this);
    player1 = nullptr;
}

void Game::openMultiplayer() {
    multiplayer = true;
    barrier = new BarrierGame(2);
    barrierWinner = new BarrierGame(2);
}

void Game::startGame() {
    if (multiplayer)
        barrier->wait();
    began = true;
}

int Game::cardBefore(Player* player) {
    if (player == player1)
        return cardBeforeP1;
    if (multiplayer && player == player2)
        return cardBeforeP2;
    return -1;
}

void Game::score() {

    //Winner:
    // 0    No one
    // 1    Player1
    // 2    Player2

    if (cardP1 >= 0 && cardP1 <= 7 && cardP2 >= 0 && cardP2 <= 7) {

        //P1 : Yargs
        //P2 : Applewoods
        if ((cardP1 == 7 && cardP2 >= 2 && cardP2 <= 6)
            || (cardP1 == 6 && cardP2 >= 1 && cardP2 <= 5 && cardP2 != 3)
            || (cardP1 == 5 && cardP2 >= 0 && cardP2 <= 4)
            || (cardP1 == 4 && (cardP2 == 1 || cardP2 == 2))
            || (cardP1 == 3 && (cardP2 == 4 || cardP2 == 6))
            || (cardP1 == 2 && (cardP2 == 1 || cardP2 == 3))
            || (cardP1 == 1 && (cardP2 == 3))
                ) { //Y
            winner = 1;
            scoreP1 += scoreBuff + 1;
            scoreBuff = 0;
        } else if (cardP1 == 1 && cardP2 == 7) {    //Y win
            winner = 1;
            scoreP1 = variante->getScoreMax();
            scoreBuff = 0;
        } else if ((cardP2 == 7 && cardP1 >= 2 && cardP1 <= 6)
                   || (cardP2 == 6 && cardP1 >= 1 && cardP1 <= 5 && cardP1 != 3)
                   || (cardP2 == 5 && cardP1 >= 0 && cardP1 <= 4)
                   || (cardP2 == 4 && (cardP1 == 1 || cardP1 == 2))
                   || (cardP2 == 3 && (cardP1 == 4 || cardP1 == 6))
                   || (cardP2 == 2 && (cardP1 == 1 || cardP1 == 3))
                   || (cardP2 == 1 && (cardP1  == 3))
                ) { //A
            winner = 2;
            scoreP2 += scoreBuff + 1;
            scoreBuff = 0;
        } else if (cardP2 == 1 && cardP1 == 7) {    //A win
            winner = 2;
            scoreP2 = variante->getScoreMax();
            scoreBuff = 0;
        } else if ((cardBeforeP1 == 6) != (cardBeforeP2 == 6)) {    //X^

            if (cardP1 == 7 && cardP2 == 7) {    //X1
                if (cardBeforeP1 == 6) {
                    winner = 1;
                    scoreP1 += scoreBuff + 1;
                    scoreBuff = 0;
                } else {
                    winner = 2;
                    scoreP2 += scoreBuff + 1;
                    scoreBuff = 0;
                }
            } else if (cardP1 == 3 && cardP2 == 3) {    //X2
                if (cardBeforeP2 == 6) {
                    winner = 2;
                    scoreP2 += scoreBuff + 1;
                    scoreBuff = 0;
                } else {
                    winner = 1;
                    scoreP1 += scoreBuff + 1;
                    scoreBuff = 0;
                }
            } else if (cardP1 == 0 && cardP2 == 0) {    //X3
                winner = 0;
                scoreBuff++;
            }

        } else {
            winner = 0;
            scoreBuff++;
        }

    }

    cardBeforeP1 = cardP1;
    cardBeforeP2 = cardP2;

}

int Game::spyCard(Player* player) {
    if ((cardBeforeP1 == 2) != (cardBeforeP2 == 2)) {
        if (player == player1) {

            if (!multiplayer) {
                do {
                    cardP2 = rand() % 8;
                } while (cardsPlayer2[cardP2] == 0);
                cardsPlayer2[cardP2]--;
            } else {
                barrierP2->wait(cardP2);
            }
            return cardP2;

        } else if (multiplayer && player == player2) {
            barrierP1->wait(cardP1);
            return cardP1;
        }
    }
    return -1;
}

int Game::getScore(Player *player) {
    if (hasBegun()) {
        if (player == player1) {
            return scoreP1;
        } else if (player == player2) {
            return scoreP2;
        }
    }
    return -1;
}

bool Game::play(Player* player, int card, VarianteAbstract* _variante) {
    if (card >= 0 && card <=7) {
        if (player == player1 && canPlay(player, card)) {
            cardP1 = card;
            cardsPlayer1[cardP1]--;
            barrierP1->wait(cardP1);
            barrier->wait();
        }

        if (multiplayer) {
            if (player == player2 && canPlay(player, card)) {
                cardP2 = card;
                cardsPlayer2[cardP2]--;
                barrierP2->wait(cardP2);
                barrier->wait();
            }
        } else if (cardP2 == -1) {
            do {
                cardP2 = rand() % 8;
            } while (cardsPlayer2[cardP2] == 0);
            cardsPlayer2[cardP2]--;
        }

        if (player == player1) {
            std::cout << "Move: p1:" << cardP1 << "-p2:" << cardP2 << " with score ";
            score();
            std::cout << scoreP1 << "-" << scoreP2 << std::endl;
            std::cout << "Winner: " << winner << std::endl;
        }

        if (player == player1 && _variante->whoWon(scoreP1, scoreP2) != 0) {
            GameManager::getInstance()->endGame(player);
        }

        //Wait for threads to get the same winner value
        barrierWinner->wait();

        if (player == player1) {
            moves.push_back(new Move(cardP1, cardP2));
            cardP1 = cardP2 = -1;
            std::cout << "Winner from P1: " << winner << std::endl;
            return (winner == 1);
        } else {
            std::cout << "Winner from P2: " << winner << std::endl;
            return (winner == 2);
        }

    }
    return false;
}

bool Game::isJoinable() {
    return (player2 == nullptr && multiplayer);
}

bool Game::hasBegun() {
    return began;
}

bool Game::isOpenMultiplayer() {
    return multiplayer;
}

bool Game::canPlay(Player *player, int nCard) {
    if (hasBegun() && nCard >= 0 && nCard <= 7 && (player == player1 || (multiplayer && player == player2))) {
        return ((player == player1 && cardsPlayer1[nCard] > 0)
            || (multiplayer && player == player2 && cardsPlayer2[nCard] > 0));
    }
    return false;
}

std::list<Move*> Game::getHistory() const {
    return moves;
}

Player* Game::getCreator() const {
    return player1;
}

Player* Game::getPlayer2() const {
    return player2;
}

VarianteAbstract *Game::getVariante() const {
    return variante;
}

Game::~Game() {
    delete player1;
    delete player2;
    delete gameManager;
    delete barrier;
    delete barrierP1;
    delete barrierP2;
    delete variante;
}