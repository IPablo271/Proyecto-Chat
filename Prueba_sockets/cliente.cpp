#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

#include "Estructuras/project.pb.h"

using namespace std;

struct Message
{
    char username[50];
    char text[100];
};

int main(int argc, char const *argv[])
{

    // PARTE DEL AGARRAR USUARIO ---------------------------------------------

    if (argc != 2)
    {
        std::cerr << "Uso: " << argv[0] << " <nombre_de_usuario>" << std::endl;
        return 1;
    }

    // Definimos el nombre de usuario como el primer argumento pasado al programa

    char *username = const_cast<char *>(argv[1]);

    // PARTE DEL AGARRAR USUARIO ---------------------------------------------
    // PARTE DEL SOCKET ------------------------------------------------------
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    struct Message message;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cout << "\n Error: Socket creation error \n";
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        cout << "\n Error: Invalid address/ Address not supported \n";
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        cout << "\n Error: Connection Failed \n";
        return -1;
    }

    // PARTE DEL SOCKET ------------------------------------------------------
    // PARTE DEL ESTRUCTURA --------------------------------------------------

    chat::UserRegister nuevo_usuario;

    nuevo_usuario.set_username(username);
    nuevo_usuario.set_ip("127.0.0.1");

    // Mostramos en forma de debugg el usuario que se va a registrar

    std::cout << "\n[ Cliente ] Usuario a registrar: " << nuevo_usuario.username() << std::endl;
    std::cout << "[ Cliente ] IP a registrar     : " << nuevo_usuario.ip() << "\n"
              << std::endl;

    // PARTE DEL ESTRUCTURA --------------------------------------------------
    // ENVIAR ESTRUCTURA -----------------------------------------------------

    // Serializar la estructura a un búfer temporal
    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    int serialized_size = nuevo_usuario.SerializeToArray(buffer, BUFFER_SIZE);

    // Impresion de buffer:
    cout << "{}{}{}Buffer: " << buffer << endl;

    // Enviar los datos a través del socket
    send(sock, buffer, serialized_size, 0);

    // send(sock, &nuevo_usuario, sizeof(nuevo_usuario), 0);

    // ENVIAR ESTRUCTURA -----------------------------------------------------

    // while (true)
    // {
    //     cout << "Escriba su mensaje (escriba 'desconectar' para salir): ";
    //     cin >> message.text;

    //     if (strcmp(message.text, "desconectar") == 0)
    //     {
    //         break;
    //     }

    //     strcpy(message.username, username);

    //     send(sock, &message, sizeof(message), 0);
    // }

    close(sock);
    return 0;
}
