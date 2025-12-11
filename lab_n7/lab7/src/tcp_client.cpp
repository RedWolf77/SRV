#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <ctime>

#define PORT 54001
#define BUFFER_SIZE 1024
#define TIMEOUT 30  // в секундах

int main() {
    int sock;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];
    std::string userInput;
    time_t lastMessageTime = time(nullptr);
    bool timeoutReached = false;

    // Создание TCP-сокета
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Ошибка создания сокета\n";
        return 1;
    }

    // Настройка адреса сервера
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    // Подключение к серверу
    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Ошибка подключения к серверу\n";
        close(sock);
        return 1;
    }

    std::cout << "Подключено к TCP-серверу на порту " << PORT << std::endl;
    std::cout << "Введите сообщения (для выхода введите 'exit')\n";
    std::cout << "Клиент автоматически отключится после " 
              << TIMEOUT << " секунд неактивности\n";

    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    while (true) {
        // Проверка таймаута
        time_t currentTime = time(nullptr);
        if (difftime(currentTime, lastMessageTime) > TIMEOUT) {
            std::cout << "\nТаймаут неактивности достигнут. Отключение...\n";
            timeoutReached = true;
            break;
        }

        std::cout << "\n> ";
        std::getline(std::cin, userInput);

        if (!userInput.empty()) {
            lastMessageTime = time(nullptr); 
            
            // Проверка на выход
            if (userInput == "exit") {
                std::cout << "Завершение работы...\n";
                send(sock, userInput.c_str(), userInput.length(), 0);
                break;
            }

            // Отправка сообщения серверу
            if (send(sock, userInput.c_str(), userInput.length(), 0) < 0) {
                std::cerr << "Ошибка отправки\n";
                break;
            }

            // Ожидание ответа от сервера
            memset(buffer, 0, BUFFER_SIZE);
            int bytesReceived = recv(sock, buffer, BUFFER_SIZE - 1, 0);
            
            if (bytesReceived > 0) {
                std::cout << "Сервер: " << buffer << std::endl;
            } else if (bytesReceived == 0) {
                std::cout << "Сервер отключился\n";
                break;
            }
        }
        
        usleep(100000); 
    }

    if (timeoutReached) {
        std::string timeoutMsg = "timeout_exit";
        send(sock, timeoutMsg.c_str(), timeoutMsg.length(), 0);
    }

    std::cout << "Отключение от сервера\n";
    close(sock);
    return 0;
}