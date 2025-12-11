#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

using namespace std;

int main() {
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(54000);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    int out = socket(AF_INET, SOCK_DGRAM, 0);
    if (out < 0) {
        cout << "Can't create socket!" << endl;
        return 1;
    }

    string msg = "";
    while (true) {
        msg.clear();
        cin >> msg;
        
        int sendOk = sendto(out, msg.c_str(), msg.size() + 1, 0, 
                           (sockaddr*)&server, sizeof(server));
        if (sendOk < 0) {
            cout << "That didn't work!" << endl;
        }
    }

    close(out); 
    return 0;
}