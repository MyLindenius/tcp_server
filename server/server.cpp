#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

int main(){
    int server_fd;
    int new_socket;
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

    //Always
    while(true){
        new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if(new_socket<0){
            perror("Failed accepting request");
            continue;
        }

        int val_read = read(new_socket, buffer, sizeof(buffer)-1); //Blocking call
        std::cout<<"Received: "<< buffer << std::endl;
        send(new_socket, buffer, strlen(buffer), 0);
        close(new_socket);
        memset(buffer, 0, sizeof(buffer)); //clear buffer
    }




}