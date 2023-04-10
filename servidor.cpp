#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <mutex>
#include <algorithm>

constexpr int PORT = 8888;

std::vector<int> clients;
std::mutex clients_mutex;

void handle_client(int client_socket)
{
    char buffer[1024];
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        int read_size = recv(client_socket, buffer, sizeof(buffer), 0);
        if (read_size <= 0)
        {
            break;
        }
        std::cout << "Cliente " << client_socket << ": " << buffer << std::endl;

        // bloquea el acceso a la lista de clientes mientras se actualiza
        std::lock_guard<std::mutex> guard(clients_mutex);
        for (auto &c : clients)
        {
            if (c != client_socket)
            { // no envía el mensaje de vuelta al cliente que lo envió
            }
            send(c, buffer, strlen(buffer), 0);
        }
    }

    // elimina el socket del cliente desconectado de la lista de clientes
    std::lock_guard<std::mutex> guard(clients_mutex);
    auto it = std::find(clients.begin(), clients.end(), client_socket);
    if (it != clients.end())
    {
        clients.erase(it);
    }

    std::cout << "Cliente desconectado: " << client_socket << std::endl;
    close(client_socket);
}

int main()
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        std::cerr << "Error al crear el socket del servidor" << std::endl;
        return 1;
    }

    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        std::cerr << "Error al configurar la opción SO_REUSEADDR" << std::endl;
        close(server_socket);
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cerr << "Error al enlazar" << std::endl;
        close(server_socket);
        return 1;
    }

    listen(server_socket, 1);
    std::cout << "Esperando conexiones en el puerto " << PORT << std::endl;

    while (true)
    {
        sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_size);
        if (client_socket < 0)
        {

            std::cerr << "Error al aceptar la conexión" << std::endl;
            continue;
        }
        // agrega el socket del nuevo cliente a la lista de clientes
        std::lock_guard<std::mutex> guard(clients_mutex);
        clients.push_back(client_socket);
        std::cout << "Cliente conectado desde " << inet_ntoa(client_addr.sin_addr) << std::endl;
        std::thread client_thread(handle_client, client_socket);
        client_thread.detach();
    }

    close(server_socket);
    return 0;
}