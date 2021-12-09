#include <sys/types.h>
#include <sys/socket.h> 
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
int PORT = 6463;
int LIMIT = 10;

int send_to(int receiver_port, char* receiver_ip, char* msg){
    int socket_fdesc;
    if (socket_fdesc = socket(AF_INET, SOCK_STREAM, 0) < 0){
        return -1;
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(receiver_port);
    inet_pton(AF_INET, receiver_ip, &(server_address.sin_addr));

    if (connect(socket_fdesc, (struct sockaddr *) &server_address, sizeof(server_address)) < 0){
        return -2;
    }
    write(socket_fdesc, msg, strlen(msg) + 1);
    close(socket_fdesc);
    return 0;
}

int main(int argc,char **argv) {
    int listen_socket_fdesc;
    

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof server_address);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htons(INADDR_ANY);
    server_address.sin_port = htons(PORT);
    
    printf("ip address: %s\n", inet_ntoa(server_address.sin_addr));
    printf("port is: %d\n", (int)ntohs(server_address.sin_port));

    if (listen_socket_fdesc = socket(AF_INET, SOCK_STREAM, 0) < 0) {
        perror("socket failed");
        exit(1);
    }
    // make port reusable 
    int yes=1; // can we do it 1 line?
    if (setsockopt(listen_socket_fdesc, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) < 0) {
        perror("setsockopt failed");
        exit(1);
    } 

    if (bind(listen_socket_fdesc, (struct sockaddr *) &server_address, sizeof(server_address)) < 0){
        perror("bind failed");
        exit(1);
    }

    if (listen(listen_socket_fdesc, LIMIT) < 0){
        perror("listen failed");
        exit(1);
    }

    int comm_fdesc = accept(listen_socket_fdesc, (struct sockaddr*) NULL, NULL); 
    printf("accept status: %d\n", comm_fdesc);

    return 0;
}
