#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>

void receive_loop(int sock) {
    char buffer[1024];
    while (true) {
        int bytes = read(sock, buffer, sizeof(buffer)-1);
        if (bytes <= 0) break;
        buffer[bytes] = 0;
        std::cout << "Friend: " << buffer << std::endl;
        std::cout.flush();
    }
}

int main(){
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0 ))<0){
        perror("Failed creating socket");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0){
        perror("Failed invalid address");
        exit(EXIT_FAILURE);
    }

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))<0){
        perror("Failed connection");
        exit(EXIT_FAILURE);
    }

    std::thread(receive_loop, sock).detach();
    
    char message[1024];
    while(true){
        std::cin.getline(message, sizeof(message));
        send(sock, message, strlen(message),0);
    }

}