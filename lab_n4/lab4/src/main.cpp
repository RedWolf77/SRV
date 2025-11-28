#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <iomanip>

class Kitchen {
private:
    std::mutex m;
    std::condition_variable cv;
    
    int dishes[3]; 
    bool cook_can_serve;
    
    std::atomic<bool> cook_fired;
    std::atomic<bool> cook_no_salary;
    std::atomic<bool> cook_quit;
    std::atomic<bool> stop_all;
    
    const int GLUTTONY;
    const int EFFICIENCY_FACTOR;
    
    int days_passed;
    std::atomic<int> total_eaten[3];
    std::atomic<int> total_cooked[3];
    
public:
    Kitchen(int gluttony, int efficiency_factor) 
        : GLUTTONY(gluttony), EFFICIENCY_FACTOR(efficiency_factor) {
        dishes[0] = dishes[1] = dishes[2] = 3000;
        cook_can_serve = true;
        cook_fired = false;
        cook_no_salary = false;
        cook_quit = false;
        stop_all = false;
        days_passed = 0;
        total_eaten[0] = total_eaten[1] = total_eaten[2] = 0;
        total_cooked[0] = total_cooked[1] = total_cooked[2] = 0;
    }
    
    void fatman_eat(int fatman_id) {
        while (!should_stop()) {
            std::unique_lock<std::mutex> lock(m);
            
            cv.wait(lock, [this]() { 
                return !cook_can_serve || should_stop(); 
            });
            
            if (should_stop()) {
                break;
            }
            
            if (dishes[fatman_id] != -1 && dishes[fatman_id] <= 0) {
                cook_fired = true;
                stop_all = true;
                cv.notify_all();
                break;
            }
            
            if (dishes[fatman_id] == -1) {
                cook_can_serve = true;
                cv.notify_all();
                lock.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                continue;
            }
            
            int eat_amount = std::min(GLUTTONY, dishes[fatman_id]);
            dishes[fatman_id] -= eat_amount;
            total_eaten[fatman_id] += eat_amount;
            
            if (total_eaten[fatman_id] > 10000) {
                dishes[fatman_id] = -1;
                
                if (dishes[0] == -1 && dishes[1] == -1 && dishes[2] == -1) {
                    cook_no_salary = true;
                    stop_all = true;
                    cv.notify_all();
                    break;
                }
            }
            
            cook_can_serve = true;
            cv.notify_all();
            
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    void cook_work() {
        auto start_time = std::chrono::steady_clock::now();
        
        while (!should_stop()) {
            std::unique_lock<std::mutex> lock(m);
            
            cv.wait(lock, [this]() { 
                return cook_can_serve || should_stop(); 
            });
            
            if (should_stop()) {
                break;
            }
            
            bool empty_plate = false;
            for (int i = 0; i < 3; i++) {
                if (dishes[i] != -1 && dishes[i] <= 0) {
                    empty_plate = true;
                    break;
                }
            }
            
            if (empty_plate) {
                cook_fired = true;
                stop_all = true;
                cv.notify_all();
                break;
            }
            
            if (dishes[0] == -1 && dishes[1] == -1 && dishes[2] == -1) {
                cook_no_salary = true;
                stop_all = true;
                cv.notify_all();
                break;
            }
            
            int added_count = 0;
            int active_plates = 0;
            for (int i = 0; i < 3; i++) {
                if (dishes[i] != -1) { 
                    dishes[i] += EFFICIENCY_FACTOR;
                    total_cooked[i] += EFFICIENCY_FACTOR;
                    added_count += EFFICIENCY_FACTOR;
                    active_plates++;
                }
            }
            
            auto current_time = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time);
            days_passed = elapsed.count();
            
            if (days_passed >= 5) {
                cook_quit = true;
                stop_all = true;
                cv.notify_all();
                break;
            }
            
            cook_can_serve = false;
            cv.notify_all();
            
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    bool should_stop() const {
        return stop_all || cook_fired || cook_no_salary || cook_quit || days_passed >= 5;
    }
    
    void print_result() {
        std::cout << "\n=== ИТОГ ===" << std::endl;
        std::cout << "Коэффициент прожорливости: " << GLUTTONY << std::endl;
        std::cout << "Коэффициент производительности: " << EFFICIENCY_FACTOR << std::endl;
        std::cout << "Финальное состояние тарелок: " << dishes[0] << ", " << dishes[1] << ", " << dishes[2] << std::endl;
        std::cout << "Всего съедено толстяками: " << total_eaten[0] << ", " << total_eaten[1] << ", " << total_eaten[2] << std::endl;
        std::cout << "Всего приготовлено Куком: " << total_cooked[0] << ", " << total_cooked[1] << ", " << total_cooked[2] << std::endl;
        std::cout << "Дней прошло: " << days_passed << std::endl;
        
        if (cook_fired) {
            std::cout << "РЕЗУЛЬТАТ: Кука уволили!" << std::endl;
        } else if (cook_no_salary) {
            std::cout << "РЕЗУЛЬТАТ: Кук не получил зарплату!" << std::endl;
        } else if (cook_quit) {
            std::cout << "РЕЗУЛЬТАТ: Кук уволился сам!" << std::endl;
        } else {
            std::cout << "РЕЗУЛЬТАТ: ??????" << std::endl;
        }
    }
};

void run_simulation(int gluttony, int efficiency_factor, const std::string& scenario_name) {
    std::cout << "\n==> СЦЕНАРИЙ " << scenario_name << std::endl;
    
    Kitchen kitchen(gluttony, efficiency_factor);
    
    std::thread fatman1(&Kitchen::fatman_eat, &kitchen, 0);
    std::thread fatman2(&Kitchen::fatman_eat, &kitchen, 1);
    std::thread fatman3(&Kitchen::fatman_eat, &kitchen, 2);
    std::thread cook(&Kitchen::cook_work, &kitchen);
    
    fatman1.join();
    fatman2.join();
    fatman3.join();
    cook.join();
    
    kitchen.print_result();
}

int main() {
    run_simulation(100, 10, "1");
    
    run_simulation(500, 500, "2");
    
    run_simulation(10, 100, "3");
    
    return 0;
}