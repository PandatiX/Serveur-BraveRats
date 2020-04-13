#ifndef SERVEURBRAVERATS_VARIANTERANDOM_HPP
#define SERVEURBRAVERATS_VARIANTERANDOM_HPP

#include <cstdlib>
#include "VarianteAbstract.hpp"

class VarianteRandom : public VarianteAbstract {
public:
    VarianteRandom() = default;
    int getScoreMax() const override {
        return 4;
    }
    void init(int (&cardsPlayer1)[8], int (&cardsPlayer2)[8]) override {
        int cards[8] = {2, 2, 2, 2, 2, 2, 2, 2};
        int tmp;
        //CardsPlayer1
        for (int i = 0; i < 8; i++) {
            do {
                tmp = rand()%8;
            } while (cards[tmp] == 0);
            cards[tmp]--;
            cardsPlayer1[tmp]++;
        }
        //CardsPlayer2
        for (int i = 0; i < 8; i++) {
            cardsPlayer2[i] = cards[i];
        }
    }
    ~VarianteRandom() override = default;
};

#endif //SERVEURBRAVERATS_VARIANTERANDOM_HPP