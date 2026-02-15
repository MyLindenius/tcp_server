#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>

std::vector<int> clients;
std::mutex clients_mutex;

void handle_client(int client_socket) {
    char buffer[1024];

    while (true) {
        int bytes = read(client_socket, buffer, sizeof(buffer)-1);
        if (bytes <= 0) break;

        buffer[bytes] = 0;

        std::lock_guard<std::mutex> lock(clients_mutex);
        for (int sock : clients) {
            if (sock != client_socket) {
                send(sock, buffer, strlen(buffer), 0);
            }
        }
    }

    close(client_socket);

    std::lock_guard<std::mutex> lock(clients_mutex);
    clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());

}

int main(){
    int server_fd;
    struct sockaddr_in address;
    char buffer[1024] = {0};

    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0); 
    if (server_fd == 0){
        perror("Failed socket");
        exit(EXIT_FAILURE);
    }

    int option = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &option, sizeof(option));
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));


    address.sin_family = AF_INET; //IPv4
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080); //host byte order -> network byte order

    // Assigne port and ip to socket
    if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0){
        perror("Failed bind");
        exit(EXIT_FAILURE);
    }


    // TODO, explain + maybe change 3 to something else?
    if(listen(server_fd, 3) < 0){
        perror("Failed listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server up and running on port 8080\n";


    while(true){
        int new_socket = accept(server_fd, nullptr, nullptr);

        {   //Used because of mutex locks
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.push_back(new_socket);
        }

        std::thread(handle_client, new_socket).detach();
    }

}