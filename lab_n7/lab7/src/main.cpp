#include <iostream>
#include <cstdlib>
#include <string>
#include <filesystem> 

const std::string CHOICE[4] = {"UDP сервер", "UDP клиент", "TCP сервер", "TCP клиент"};

int main() {
    std::string exeDir = std::filesystem::current_path().string();
    
    std::cout << "Выберите программу для запуска:\n";
    std::cout << "1 - UDP сервер\n";
    std::cout << "2 - UDP клиент\n";
    std::cout << "3 - TCP сервер\n";
    std::cout << "4 - TCP клиент\n";
    std::cout << "Ваш выбор: ";
    
    int choice;
    std::cin >> choice;
    
    std::string command;
    switch(choice) {
        case 1: command = exeDir + "/lab_n7/build_release/udp_server"; break;
        case 2: command = exeDir + "/lab_n7/build_release/udp_client"; break;
        case 3: command = exeDir + "/lab_n7/build_release/tcp_server"; break;
        case 4: command = exeDir + "/lab_n7/build_release/tcp_client"; break;
        default: 
            std::cout << "Неверный выбор\n";
            return 1;
    }
    
    std::cout << CHOICE[choice - 1]  << " запущен!" << std::endl;
    system(command.c_str());
    
    return 0;
}