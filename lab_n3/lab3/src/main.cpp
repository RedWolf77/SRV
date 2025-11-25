#include <iostream>
#include <thread>
#include <mutex>
#include <string>

int coins = 101;
int Bob_coins = 0;
int Tom_coins = 0;
std::mutex m;

void coin_sharing(int& thief_coins, int& companion_coins) {
    while (true) {
        m.lock();
        
        if (coins <= 1) {
            if (thief_coins != companion_coins) {
                if (thief_coins > companion_coins) companion_coins++;
                else thief_coins++;
                coins--;
            }
            m.unlock();
            break;
        }
        
        if (thief_coins <= companion_coins) {
            coins--;
            thief_coins++;
        }
        
        m.unlock();
    }
}

int main() {
    std::cout << "\nВсего монет (изначально): " << coins << std::endl;
    
    std::thread bob_thread(coin_sharing, std::ref(Bob_coins), std::ref(Tom_coins));
    std::thread tom_thread(coin_sharing, std::ref(Tom_coins), std::ref(Bob_coins));
    
    bob_thread.join();
    tom_thread.join(); 

    std::cout << std::endl;
    std::cout << "Итоговый результат:" << std::endl;
        
    std::cout << "Боб: " << Bob_coins << " монет" << std::endl;
    std::cout << "Том: " << Tom_coins << " монет" << std::endl;
    
    if (coins > 0) std::cout << "Покойнику: 1 монета" << std::endl;
    else std::cout << "Покойнику: 0 монет" << std::endl;
    
    int total = Bob_coins + Tom_coins + coins;
    std::cout << "Всего монет (подсчет): " << total << "\n" << std::endl;
    
    return 0;
}