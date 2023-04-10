#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <thread>

#include "Estructuras/project.pb.h"

using namespace std;

constexpr int PORT = 8888;

// Esta función se ejecutará en un hilo separado para recibir mensajes del servidor
void receive_messages(int client_socket)
{
    char recv_buffer[1024 + 1]; // Agregar un espacio adicional para el carácter de terminación nula

    while (true)
    {
        // Limpiar el búfer antes de recibir un mensaje nuevo
        memset(recv_buffer, 0, sizeof(recv_buffer));

        // Recibir el mensaje del servidor
        int read_size = recv(client_socket, recv_buffer, sizeof(recv_buffer) - 1, 0); // Leer hasta un byte menos para dejar espacio para el carácter de terminación nula

        // Si no se pudo recibir ningún mensaje, salir del bucle
        if (read_size <= 0)
        {
            break;
        }

        // Agregar el carácter de terminación nula al final del mensaje recibido
        recv_buffer[read_size] = '\0';

        // Imprimir el mensaje recibido en la consola
        std::cout << recv_buffer << std::endl;
    }
}

// Método para chatear con todos los usuarios
void broadcast(int client_socket, std::string username)
{

    cout << "\n\nConectandote al chat con todos los usuarios...\n\n"
         << endl;

    // Creamos el thread para recibir mensajes que nosotros enviamos:

    std::thread receive_thread(receive_messages, client_socket);

    // Separar el hilo principal del hilo de recepción

    receive_thread.detach();

    // Bucle principal del cliente para enviar mensajes al servidor

    char buffer[1024];

    while (true)
    {

        // Limpiar el búfer antes de enviar un mensaje nuevo

        memset(buffer, 0, sizeof(buffer));

        // Pedir al usuario que ingrese un mensaje

        // std::cout << "Mensaje: ";
        std::cin.getline(buffer, sizeof(buffer));

        // Concatenar el nombre de usuario con el mensaje ingresado por el usuario
        std::string message = username + ": " + buffer;

        // Enviar el mensaje al servidor

        int send_result = send(client_socket, message.c_str(), message.length(), 0);

        if (send_result < 0)
        {
            std::cerr << "Error al enviar el mensaje" << std::endl;
            break;
        }
    }

    close(client_socket);
}

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        std::cerr << "Uso: " << argv[0] << " <nombre_de_usuario>" << std::endl;
        return 1;
    }

    // Definimos el nombre de usuario como el primer argumento pasado al programa

    std::string username(argv[1]);

    // Creamos el socket del cliente

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Verificamos que el socket se haya creado correctamente

    if (client_socket == -1)
    {
        std::cerr << "Error al crear el socket del cliente" << std::endl;
        return 1;
    }

    // La estructura sockaddr_in se utiliza para representar una dirección de socket de Internet

    sockaddr_in direccionSocketServidor;

    // El miembro sin_family especifica la familia de direcciones, que en este caso es AF_INET

    direccionSocketServidor.sin_family = AF_INET;

    // El miembro sin_port especifica el número de puerto en orden de bytes de red

    direccionSocketServidor.sin_port = htons(PORT);

    // El miembro sin_addr especifica la dirección IP en orden de bytes de red

    direccionSocketServidor.sin_addr.s_addr = htons(INADDR_ANY);

    // Creamos la estructura "UserRegister" para enviarla al servidor y registrar al usuario usando el #include "Estructuras/project.pb.h"

    chat::UserRegister nuevo_usuario;

    nuevo_usuario.set_username(username);
    nuevo_usuario.set_ip("IP.IP.CorregirIP");

    // Mostramos en forma de debugg el usuario que se va a registrar

    std::cout << "\n[ Cliente ] Usuario a registrar: " << nuevo_usuario.username() << std::endl;
    std::cout << "[ Cliente ] IP a registrar     : " << nuevo_usuario.ip() << "\n"
              << std::endl;

    // Establecemos la conexión con el servidor

    int conexion = connect(client_socket, (sockaddr *)&direccionSocketServidor, sizeof(direccionSocketServidor));

    // Verificamos que la conexión se haya establecido correctamente

    if (conexion == -1)
    {
        std::cerr << "Error al establecer la conexión con el servidor" << std::endl;
        return 1;
    }

    std::cout << "[ Cliente ] Usuario conectado al servidor\n"
              << std::endl;

    int option;
    bool exit = false;

    while (!exit)
    {
        cout << "Menu de opciones:" << endl;
        cout << "1. Chatear con todos los usuarios (broadcasting)." << endl;
        cout << "7. Salir." << endl;
        cout << "Ingrese el numero de la opcion deseada: ";
        cin >> option;

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (option)
        {
        case 1:
            broadcast(client_socket, username);
            break;
        case 7:
            exit = true;
            break;
        default:
            cout << "Opcion invalida." << endl;
            break;
        }

        cout << endl;
    }

    cout << "Saliendo del programa..." << endl;

    return 0;
}