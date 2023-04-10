#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "Estructuras/project.pb.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Uso: ./cliente nombre_usuario" << std::endl;
        return 1;
    }

    std::string nombre_usuario(argv[1]);
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        std::cerr << "Error al crear el socket" << std::endl;
        return 1;
    }

    sockaddr_in servidor_addr;
    servidor_addr.sin_family = AF_INET;
    servidor_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servidor_addr.sin_port = htons(8080);

    if (connect(sock, (sockaddr *)&servidor_addr, sizeof(servidor_addr)) == -1)
    {
        std::cerr << "Error al conectar con el servidor" << std::endl;
        return 1;
    }

    chat::UserRegister cliente_temporal;
    cliente_temporal.set_username(nombre_usuario);
    cliente_temporal.set_ip(inet_ntoa(servidor_addr.sin_addr));

    std::string buffer;
    cliente_temporal.SerializeToString(&buffer);
    send(sock, buffer.c_str(), buffer.size(), 0);

    close(sock);
    return 0;
}
