#ifndef SERVEURBRAVERATS_GAME_HPP
#define SERVEURBRAVERATS_GAME_HPP

#include <netinet/in.h>
#include <random>

#include "Player.hpp"
#include "Move.hpp"
#include "barriers/BarrierGame.hpp"
#include "barriers/BarrierCard.hpp"
#include "variantes/VarianteDefault.hpp"
#include "variantes/VarianteFastRat.hpp"
#include "variantes/VariantePrincess.hpp"
#include "variantes/VarianteRandom.hpp"
#include "variantes/VarianteTraitor.hpp"

class Game {
private:
    int cardsPlayer1[8]{}, cardsPlayer2[8]{};
    int cardP1, cardBeforeP1, cardP2, cardBeforeP2;
    int scoreP1, scoreP2, scoreBuff, winner;
    bool multiplayer, began;
    GameManager *gameManager;
    Player *player1, *player2;
    BarrierGame *barrier, *barrierWinner;
    BarrierCard *barrierP1, *barrierP2;
    VarianteAbstract *variante;
    std::list<Move*> moves;

    //random number generation
    std::random_device rd;
    std::mt19937 mt;
    std::uniform_int_distribution<int> dist;

public:
    Game(Player*);

    int cardBefore(Player*);
    int spyCard(Player*);
    int getScore(Player*);
    int* getCards(Player*);
    bool isJoinable();
    bool hasBegun() const;
    bool isOpenMultiplayer() const;
    bool canPlay(Player*, int);
    bool play(Player*, int, VarianteAbstract*);
    std::list<Move*> getHistory() const;
    Player *getCreator() const;
    Player *getPlayer2() const;
    Game *setSecondPlayer(Player*);
    VarianteAbstract *getVariante() const;

    void startGame();
    void endGame();
    void openMultiplayer();
    void score();
    void leave(Player*);
    void setVariante(VarianteAbstract*);

    ~Game();
};

#endif //SERVEURBRAVERATS_GAME_HPP