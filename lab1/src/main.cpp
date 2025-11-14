#include <iostream>
#include <chrono>
#include <thread>

void func(int iterations) {
    volatile long result = 1; // volatile чтобы компилятор не удалил вычисления
    
    for (int j = 0; j < iterations; j++) {
        result = 1;
        
        for (int i = 1; i <= 100; i++) {
            result *= i;
        }
    }
}

int main() {
    const int iterations = 10000000;
    
    // Параллельная версия
    auto start_parallel = std::chrono::high_resolution_clock::now();
    
    std::thread t1(func, iterations);
    std::thread t2(func, iterations);
    t1.join();
    t2.join();
    
    auto end_parallel = std::chrono::high_resolution_clock::now();
    
    // Последовательная версия
    auto start_sequential = std::chrono::high_resolution_clock::now();
    
    func(iterations);
    func(iterations);
    
    auto end_sequential = std::chrono::high_resolution_clock::now();
    
    auto parallel_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_parallel - start_parallel);
    auto sequential_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_sequential - start_sequential);
    
    std::cout << "Parallel time: " << parallel_time.count() << " ms" << std::endl;
    std::cout << "Sequential time: " << sequential_time.count() << " ms" << std::endl;
    
    return 0;
}