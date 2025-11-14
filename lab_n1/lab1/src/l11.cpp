


#include <iostream>
#include <time.h>
#include <thread>
#include <mutex>
#include <string>

std::mutex cout_mutex;

void func(unsigned long long int number) {
    unsigned long long int result = 1;

    for (unsigned long long int i = 1; i <= number; i++) {
        result *= i;
    }

    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << "Factorial of " << number << " = " << result << std::endl;
}

int main() {
    int n = 10;

    clock_t start = clock();

    std::thread thread1(func, 5);
    std::thread thread2(func, 5);
    thread1.join();
    thread2.join();


    clock_t end = clock();
    double seconds = (double)(end - start) / CLOCKS_PER_SEC;

    std::cout << "Seconds: " << seconds << std::endl;
    std::cout << "Press Enter to continue...";
    std::cin.get();

    return 0;
}

