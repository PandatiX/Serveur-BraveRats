#ifndef SERVEURBRAVERATS_VARIANTEFASTRAT_HPP
#define SERVEURBRAVERATS_VARIANTEFASTRAT_HPP

#include "VarianteAbstract.hpp"

class VarianteFastRat : public VarianteAbstract {
public:
    VarianteFastRat() = default;
    int getScoreMax() const {
        return 3;
    }
    void init(int (&cardsPlayer1)[8], int (&cardsPlayer2)[8]) {
        for (int i = 0; i < 8; i++) {
            cardsPlayer1[i]++;
            cardsPlayer2[i]++;
        }
    }
    ~VarianteFastRat() = default;
};

#endif //SERVEURBRAVERATS_VARIANTEFASTRAT_HPP