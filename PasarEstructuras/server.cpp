#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "Estructuras/project.pb.h"

void imprimir_usuarios_registrados(const std::vector<chat::UserRegister> &usuarios_registrados)
{
    for (const auto &usuario : usuarios_registrados)
    {
        std::cout << "Usuario: " << usuario.username() << ", IP: " << usuario.ip() << std::endl;
    }
}

int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        std::cerr << "Error al crear el socket" << std::endl;
        return 1;
    }

    sockaddr_in servidor_addr;
    servidor_addr.sin_family = AF_INET;
    servidor_addr.sin_addr.s_addr = INADDR_ANY;
    servidor_addr.sin_port = htons(8080);

    if (bind(sock, (sockaddr *)&servidor_addr, sizeof(servidor_addr)) == -1)
    {
        std::cerr << "Error al asignar dirección y puerto al socket" << std::endl;
        return 1;
    }

    if (listen(sock, 10) == -1)
    {
        std::cerr << "Error al escuchar conexiones entrantes" << std::endl;
        return 1;
    }

    std::vector<chat::UserRegister> usuarios_registrados;

    while (true)
    {
        sockaddr_in cliente_addr;
        socklen_t cliente_len = sizeof(cliente_addr);
        int cliente_sock = accept(sock, (sockaddr *)&cliente_addr, &cliente_len);
        if (cliente_sock == -1)
        {
            std::cerr << "Error al aceptar conexión entrante" << std::endl;
            continue;
        }

        char buffer[4096];
        ssize_t bytes_recibidos = recv(cliente_sock, buffer, sizeof(buffer), 0);
        if (bytes_recibidos == -1)
        {
            std::cerr << "Error al recibir datos del cliente" << std::endl;
            close(cliente_sock);
            continue;
        }

        chat::UserRegister nuevo_usuario;
        nuevo_usuario.ParseFromArray(buffer, bytes_recibidos);

        usuarios_registrados.push_back(nuevo_usuario);
        imprimir_usuarios_registrados(usuarios_registrados);

        close(cliente_sock);
    }

    close(sock);
    return 0;
}
