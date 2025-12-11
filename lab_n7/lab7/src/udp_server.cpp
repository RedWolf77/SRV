#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

using namespace std;

int main() {
    // Создаем UDP-сокет
    int in = socket(AF_INET, SOCK_DGRAM, 0);
    if (in < 0) {
        cout << "Can't create socket!" << endl;
        return 1;
    }

    // Настройка адреса сервера
    sockaddr_in serverHint;
    serverHint.sin_addr.s_addr = INADDR_ANY; 
    serverHint.sin_family = AF_INET;
    serverHint.sin_port = htons(54000);

    // Привязка сокета
    int bindResult = ::bind(in, (sockaddr*)&serverHint, sizeof(serverHint));
    if (bindResult < 0) {
        cout << "Can't bind socket!" << endl;
        close(in); 
        return 1;
    }

    sockaddr_in client;
    socklen_t clientLength = sizeof(client);
    char buf[1024];

    while (true) {
        memset(&client, 0, clientLength); 
        memset(buf, 0, 1024);

        int bytesIn = recvfrom(in, buf, 1024, 0, (sockaddr*)&client, &clientLength);
        if (bytesIn < 0) {
            cout << "Error receiving from client" << endl;
            continue;
        }

        char clientIP[256];
        inet_ntop(AF_INET, &client.sin_addr, clientIP, 256);
        cout << "Message recv from " << clientIP << " : " << buf << endl;
    }

    close(in); 
    return 0;
}