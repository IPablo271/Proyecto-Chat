#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

using namespace std;

struct Message {
    char username[50];
    char text[100];
};

int main(int argc, char const *argv[]) {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char username[50];
    struct Message message;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "\n Error: Socket creation error \n";
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        cout << "\n Error: Invalid address/ Address not supported \n";
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        cout << "\n Error: Connection Failed \n";
        return -1;
    }

    cout << "Ingrese su nombre de usuario: ";
    cin >> username;

    while (true) {
        cout << "Escriba su mensaje (escriba 'desconectar' para salir): ";
        cin >> message.text;

        if (strcmp(message.text, "desconectar") == 0) {
            break;
        }

        strcpy(message.username, username);

        send(sock, &message, sizeof(message), 0);
    }

    close(sock);
    return 0;
}
