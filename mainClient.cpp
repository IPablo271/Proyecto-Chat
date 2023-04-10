#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <thread>


using namespace std;


constexpr int PORT = 8888;

// Esta función se ejecutará en un hilo separado para recibir mensajes del servidor
void receive_messages(int client_socket) {
    char buffer[1024];
    while (true) {
        // Limpiar el búfer antes de recibir un mensaje nuevo
        memset(buffer, 0, sizeof(buffer));
        // Recibir el mensaje del servidor
        int read_size = recv(client_socket, buffer, sizeof(buffer), 0);
        // Si no se pudo recibir ningún mensaje, salir del bucle
        if (read_size <= 0) {
            break;
        }
        // Imprimir el mensaje recibido en la consola
        std::cout << buffer << std::endl;
    }
}

// Método para chatear con todos los usuarios
void broadcast(int client_socket, std::string username) {

    cout << "Conectandote al chat con todos los usuarios..." << endl;

    // Creamos el thread para recibir mensajes que nosotros enviamos:

    std::thread receive_thread(receive_messages, client_socket);

    // Separar el hilo principal del hilo de recepción

    receive_thread.detach();

    // Bucle principal del cliente para enviar mensajes al servidor

    char buffer[1024];

    while (true) {

        // Limpiar el búfer antes de enviar un mensaje nuevo

        memset(buffer, 0, sizeof(buffer));

        // Pedir al usuario que ingrese un mensaje

        std::cout << "Mensaje: ";
        std::cin.getline(buffer, sizeof(buffer));

        // Concatenar el nombre de usuario con el mensaje ingresado por el usuario
        std::string message = username + ": " + buffer;

        // Enviar el mensaje al servidor

        int send_result = send(client_socket, message.c_str(), message.length(), 0);

        if (send_result < 0) {
            std::cerr << "Error al enviar el mensaje" << std::endl;
            break;
        }
    }

    close(client_socket);


}

// Método para enviar y recibir mensajes directos
void directMessages() {
    // TODO: Implementar método
    cout << "Enviando y recibiendo mensajes directos..." << endl;
}

// Método para cambiar de status
void changeStatus() {
    // TODO: Implementar método
    cout << "Cambiando de status..." << endl;
}

// Método para listar usuarios conectados
void listUsers() {
    // TODO: Implementar método
    cout << "Listando usuarios conectados..." << endl;
}

// Método para desplegar información de un usuario en particular
void userInfo() {
    // TODO: Implementar método
    cout << "Desplegando información de usuario en particular..." << endl;
}

// Método de ayuda
void help() {
    // TODO: Implementar método
    cout << "Mostrando ayuda..." << endl;
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <nombre_de_usuario>" << std::endl;
        return 1;
    }

    // Definimos el nombre de usuario como el primer argumento pasado al programa

    std::string username(argv[1]);

    // Creamos el socket del cliente

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Verificamos que el socket se haya creado correctamente

    if (client_socket == -1) {
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

    // Establecemos la conexión con el servidor

    int conexion = connect(client_socket, (sockaddr *) &direccionSocketServidor, sizeof(direccionSocketServidor));

    // Verificamos que la conexión se haya establecido correctamente

    if (conexion == -1) {
        std::cerr << "Error al establecer la conexión con el servidor" << std::endl;
        return 1;
    }

    std::cout << "Usuario conectado al servidor" << std::endl;


    int option;
    bool exit = false;

    while (!exit) {
        cout << "Menu de opciones:" << endl;
        cout << "1. Chatear con todos los usuarios (broadcasting)." << endl;
        cout << "2. Enviar y recibir mensajes directos, privados, aparte del chat general." << endl;
        cout << "3. Cambiar de status." << endl;
        cout << "4. Listar los usuarios conectados al sistema de chat." << endl;
        cout << "5. Desplegar información de un usuario en particular." << endl;
        cout << "6. Ayuda." << endl;
        cout << "7. Salir." << endl;
        cout << "Ingrese el numero de la opcion deseada: ";
        cin >> option;

        switch (option) {
            case 1:
                broadcast(client_socket, username);
                break;
            case 2:
                directMessages();
                break;
            case 3:
                changeStatus();
                break;
            case 4:
                listUsers();
                break;
            case 5:
                userInfo();
                break;
            case 6:
                help();
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
