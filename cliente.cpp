#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <thread>

constexpr int PORT = 8888;

void receive_messages(int client_socket) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int read_size = recv(client_socket, buffer, sizeof(buffer), 0);
        if (read_size <= 0) {
            break;
        }
        std::cout << buffer << std::endl;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <nombre_de_usuario>" << std::endl;
        return 1;
    }

    std::string username(argv[1]);

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "Error al crear el socket del cliente" << std::endl;
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error al conectar con el servidor" << std::endl;
        return 1;
    }

    std::cout << "Conectado al servidor" << std::endl;

    std::thread receive_thread(receive_messages, client_socket);
    receive_thread.detach();

    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        std::cout << "Ingrese un mensaje: ";
        std::cin.getline(buffer, sizeof(buffer));
        std::string message = username + ": " + buffer;
        if (send(client_socket, message.c_str(), message.length(), 0) < 0) {
            std::cerr << "Error al enviar el mensaje" << std::endl;
            break;
        }
    }

    close(client_socket);
    return 0;
}
