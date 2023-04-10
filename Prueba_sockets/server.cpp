#include <iostream>
#include <thread>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

struct Message {
    char username[50];
    char message[256];
};

void handle_client(int client_socket) {
    char buffer[256] = {0};
    Message msg;
    int valread;
    while (true) {
        valread = read(client_socket, &msg, sizeof(Message));
        if (valread <= 0) {
            // Client disconnected
            close(client_socket);
            cout << "Client disconnected" << endl;
            break;
        } else {
            cout << msg.username << ": " << msg.message << endl;
        }
    }
}

int main(int argc, char const *argv[]) {
    int server_socket, client_socket, opt = 1;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Create server socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        cerr << "Socket creation error" << endl;
        return 1;
    }

    // Set socket options
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        cerr << "Setsockopt error" << endl;
        return 1;
    }

    // Bind socket to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);
    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        cerr << "Bind failed" << endl;
        return 1;
    }

    // Listen for clients
    if (listen(server_socket, 3) < 0) {
        cerr << "Listen error" << endl;
        return 1;
    }

    vector<thread> client_threads;

    while (true) {
        // Accept incoming connection
        if ((client_socket = accept(server_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            cerr << "Accept error" << endl;
            continue;
        }

        cout << "New client connected" << endl;

        // Create a new thread to handle the communication with the client
        client_threads.emplace_back(handle_client, client_socket);
    }

    // Join all client threads
    for (auto& t : client_threads) {
        t.join();
    }

    return 0;
}
