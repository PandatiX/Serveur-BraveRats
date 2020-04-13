#ifndef SERVEURBRAVERATS_VARIANTETRAITOR_HPP
#define SERVEURBRAVERATS_VARIANTETRAITOR_HPP

#include <cstdlib>
#include "VarianteAbstract.hpp"

class VarianteTraitor : public VarianteAbstract {
public:
    VarianteTraitor() = default;
    int getScoreMax() const override {
        return 4;
    }
    void init(int (&cardsPlayer1)[8], int (&cardsPlayer2)[8]) override {
        for (int i = 0; i < 8; i++) {
            cardsPlayer1[i]++;
            cardsPlayer2[i]++;
        }
        int posCardP1 = rand()%8;
        int posCardP2 = rand()%8;
        cardsPlayer1[posCardP1]++;
        cardsPlayer2[posCardP1]--;
        cardsPlayer1[posCardP2]--;
        cardsPlayer2[posCardP2]++;
    }
    ~VarianteTraitor() override = default;
};

#endif //SERVEURBRAVERATS_VARIANTETRAITOR_HPP