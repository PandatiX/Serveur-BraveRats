#ifndef SERVEURBRAVERATS_BARRIERGAME_HPP
#define SERVEURBRAVERATS_BARRIERGAME_HPP

#include <mutex>
#include <condition_variable>

class BarrierGame {
private:
    int counter, waiting, thread_counter;
    std::mutex m;
    std::condition_variable cv;

public:
    BarrierGame(int _thread_counter) {
        counter = 0;
        thread_counter = _thread_counter;
    }
    void wait() {
        std::unique_lock<std::mutex> lk(m);
        ++counter;
        ++waiting;
        cv.wait(lk, [&]{return counter >= thread_counter;});
        cv.notify_all();
        if (--waiting == 0)
            counter = 0;
        lk.unlock();
    }
};

#endif //SERVEURBRAVERATS_BARRIERGAME_HPP