#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <ctime>

#define PORT 54001  
#define BUFFER_SIZE 1024
#define TIMEOUT 30  // в секундах

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    char buffer[BUFFER_SIZE];
    time_t lastActivityTime;

    // Создание TCP-сокета
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Ошибка создания сокета\n";
        return 1;
    }

    // Настройка адреса сервера
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Привязка сокета
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Ошибка привязки\n";
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Ошибка listen\n";
        close(serverSocket);
        return 1;
    }

    std::cout << "TCP-сервер запущен на порту " << PORT << std::endl;
    std::cout << "Ожидание подключений...\n";
    std::cout << "Сервер автоматически отключит неактивного клиента через " 
              << TIMEOUT << " секунд\n";

    // Принять подключения
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
    if (clientSocket < 0) {
        std::cerr << "Ошибка accept\n";
        close(serverSocket);
        return 1;
    }

    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
    std::cout << "Клиент подключен: " << clientIP << ":" << ntohs(clientAddr.sin_port) << std::endl;

    lastActivityTime = time(nullptr); // Ыремя последней активности

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);

        // Проверка таймаута
        time_t currentTime = time(nullptr);
        if (difftime(currentTime, lastActivityTime) > TIMEOUT) {
            std::cout << "Таймаут неактивности. Отключение клиента.\n";
            break;
        }

        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        // Прием сообщения
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
        
        if (bytesReceived > 0) {
            lastActivityTime = time(nullptr);  
            
            std::cout << "Клиент: " << buffer << std::endl;

            // Проверка на выход
            if (strcmp(buffer, "exit") == 0) {
                std::cout << "Клиент запросил отключение\n";
                break;
            }

            // Отправить ответ о получиении сообщения
            std::string response = "получено сообщение от клиента: ";
            response += buffer;
            send(clientSocket, response.c_str(), response.length(), 0);
            
        } else if (bytesReceived == 0) {
            std::cout << "Клиент отключился\n";
            break;
        }
    }

    std::cout << "Завершение работы сервера\n";
    close(clientSocket);
    close(serverSocket);
    return 0;
}