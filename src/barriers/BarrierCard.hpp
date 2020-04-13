#ifndef SERVEURBRAVERATS_BARRIERCARD_HPP
#define SERVEURBRAVERATS_BARRIERCARD_HPP

#include <mutex>
#include <condition_variable>
#include <iostream>

class BarrierCard {
private:
    std::mutex m;
    std::condition_variable cv;

public:
    BarrierCard() = default;
    void wait(int& card) {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [&]{return card != -1;});
        cv.notify_all();
        lk.unlock();
    }
};

#endif //SERVEURBRAVERATS_BARRIERCARD_HPP