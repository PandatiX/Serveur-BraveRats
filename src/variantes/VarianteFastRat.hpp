#ifndef SERVEURBRAVERATS_VARIANTEFASTRAT_HPP
#define SERVEURBRAVERATS_VARIANTEFASTRAT_HPP

#include "VarianteAbstract.hpp"

class VarianteFastRat : public VarianteAbstract {
public:
    VarianteFastRat() = default;
    int getScoreMax() const override {
        return 3;
    }
    void init(int (&cardsPlayer1)[8], int (&cardsPlayer2)[8]) override {
        for (int i = 0; i < 8; i++) {
            cardsPlayer1[i]++;
            cardsPlayer2[i]++;
        }
    }
    ~VarianteFastRat() override = default;
};

#endif //SERVEURBRAVERATS_VARIANTEFASTRAT_HPP