#ifndef SERVEURBRAVERATS_VARIANTEDEFAULT_HPP
#define SERVEURBRAVERATS_VARIANTEDEFAULT_HPP

#include "VarianteAbstract.hpp"

class VarianteDefault : public VarianteAbstract {
public:
    VarianteDefault() = default;
    int getScoreMax() const {
        return 4;
    }
    void init(int (&cardsPlayer1)[8], int (&cardsPlayer2)[8]) {
        for (int i = 0; i < 8; i++) {
            cardsPlayer1[i] = 1;
            cardsPlayer2[i] = 1;
        }
    }
    ~VarianteDefault() = default;
};

#endif //SERVEURBRAVERATS_VARIANTEDEFAULT_HPP