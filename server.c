#include <stdio.h>
#include <stdlib.h>
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
//SO_REUSEPORT instead of SO_REUSEADDR? 

int main(int argc, char const *argv[]){
    int limit = 10;
    char msg[100];
    char hname[30];
    struct sockaddr_in peer;
    gethostname(hname, sizeof(hname));
    printf("hostname: %s\n", hname);
    int listen_socket_fdesc = socket(AF_INET, SOCK_STREAM, 0);
    printf("Listen socket desc status: %d\n", listen_socket_fdesc);

    int yes=1; // can we do it 1 line?
    if (setsockopt(listen_socket_fdesc, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
        perror("setsockopt");
        exit(1);
    } 

    struct sockaddr_in server;
    memset(&server, 0, sizeof server);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htons(INADDR_ANY);
    server.sin_port = htons(6463);

    int b = bind(listen_socket_fdesc, (struct sockaddr *) &server, sizeof(server));
    printf("bind status: %d\n", b);
    int l = listen(listen_socket_fdesc, limit);
    printf("listen status: %d\n", l);
    int comm_fdesc = accept(listen_socket_fdesc, (struct sockaddr*) NULL, NULL); 
    printf("accept status: %d\n", comm_fdesc);





    socklen_t len;
    struct sockaddr_storage addr;
    memset(&addr, 0, sizeof addr);
    char ipstr[INET6_ADDRSTRLEN];
    int port;
    len = sizeof addr;
    getpeername(comm_fdesc, (struct sockaddr*)&addr, &len);
    // deal with both IPv4 and IPv6:
    if (addr.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in *)&addr;
        port = ntohs(s->sin_port);
        inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
    } else { // AF_INET6
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
        port = ntohs(s->sin6_port);
        inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
    }
    printf("peer IP: %s\n", ipstr);
    printf("peer port: %d\n", port);
    while (1){
        memset(msg, 0, sizeof(msg));
        read(comm_fdesc, msg, 100); 
        //printf("ECHO: %s\n", msg);
        write(comm_fdesc, msg, strlen(msg) + 1);
    }
    return 0;
}