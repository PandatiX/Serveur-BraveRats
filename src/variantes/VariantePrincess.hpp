#ifndef SERVEURBRAVERATS_VARIANTEPRINCESS_HPP
#define SERVEURBRAVERATS_VARIANTEPRINCESS_HPP

#include <cstdlib>
#include "VarianteAbstract.hpp"

class VariantePrincess : public VarianteAbstract {
public:
    VariantePrincess() = default;
    int getScoreMax() const {
        return 4;
    }
    void init(int (&cardsPlayer1)[8], int (&cardsPlayer2)[8]) {
        for (int i = 0; i < 8; i++) {
            cardsPlayer1[i] = 1;
            cardsPlayer2[i] = 1;
        }
        bool sw = (bool)rand()%2;
        if (sw) {
            cardsPlayer1[1]++;
            cardsPlayer2[1]--;
            cardsPlayer1[7]--;
            cardsPlayer2[7]++;
        } else {
            cardsPlayer1[1]--;
            cardsPlayer2[1]++;
            cardsPlayer1[7]++;
            cardsPlayer2[7]--;
        }
    }
};

#endif //SERVEURBRAVERATS_VARIANTEPRINCESS_HPP
