#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
//replace bzero with memset
//should i worry about local ports?
//replace write and read with send and rcv
// close /shutdown to close socket
//multi -poll

int main(int argc, char const *argv[]){
    int limit = 10;
    char msg[100];
    int listen_socket_fdesc = socket(AF_INET, SOCK_STREAM, 0);
    printf("Listen socket desc status: %d\n", listen_socket_fdesc);

    struct sockaddr_in server;
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htons(INADDR_ANY);
    server.sin_port = htons(6463);

    int b = bind(listen_socket_fdesc, (struct sockaddr *) &server, sizeof(server));
    printf("bind status: %d\n", b);
    // reusing port
    /*
    int yes=1; //
    // lose the pesky "Address already in use" error message
    if (setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
        perror("setsockopt");
        exit(1);
    } 
    */
    int l = listen(listen_socket_fdesc, limit);
    printf("listen status: %d\n", l);
    int comm_fdesc = accept(listen_socket_fdesc, (struct sockaddr*) NULL, NULL); 
    printf("accept status: %d\n", comm_fdesc);
    while (1){
        bzero(msg, 100);
        read(comm_fdesc, msg, 100); 
        //printf("ECHO: %s\n", msg);
        write(comm_fdesc, msg, strlen(msg) + 1);
    }
    return 0;
}