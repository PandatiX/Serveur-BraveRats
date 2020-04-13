#ifndef SERVEURBRAVERATS_VARIANTEABSTRACT_HPP
#define SERVEURBRAVERATS_VARIANTEABSTRACT_HPP

class VarianteAbstract {
public:
    virtual void init(int (&cardsPlayer1)[8], int (&cardsPlayer2)[8]) = 0;
    virtual int getScoreMax() const = 0;
    int whoWon(int scoreP1, int scoreP2) const {
        if (scoreP1 >= getScoreMax())
            return 1;
        else if (scoreP2 >= getScoreMax())
            return 2;
        else return 0;
    }
    virtual ~VarianteAbstract() = default;
};

#endif //SERVEURBRAVERATS_VARIANTEABSTRACT_HPP