#ifndef SERVEURBRAVERATS_VARIANTEDEFAULT_HPP
#define SERVEURBRAVERATS_VARIANTEDEFAULT_HPP

#include "VarianteAbstract.hpp"

class VarianteDefault : public VarianteAbstract {
public:
    VarianteDefault() = default;
    int getScoreMax() const override {
        return 4;
    }
    void init(int (&cardsPlayer1)[8], int (&cardsPlayer2)[8]) override {
        for (int i = 0; i < 8; i++) {
            cardsPlayer1[i] = 1;
            cardsPlayer2[i] = 1;
        }
    }
    ~VarianteDefault() override = default;
};

#endif //SERVEURBRAVERATS_VARIANTEDEFAULT_HPP