#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

int main(){
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0 ))<0){
        perror("Failed creating socket");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    


}