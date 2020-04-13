#include "src/Game.hpp"

class Manager{
private:
    static std::shared_ptr<Manager> singleton;
    Manager() = default;

public:
    static std::shared_ptr<Manager> getInstance() {
        static std::shared_ptr<Manager> d(new Manager);
        std::cout << "Count: " << d.use_count() << std::endl;
        return d;
    }

    void Print() {
        std::cout << "PRINT" << std::endl;
    }
};

int main(int argc, char **argv) {

    srand(time(nullptr));

    GameManager *gameManager = GameManager::getInstance();
    return gameManager->start();

    //return EXIT_SUCCESS;

}