
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "p2p.h"


int main(int argc,char **argv) {
    int MYPORT;
    scanf("%d", &MYPORT);

    int listen_socket_fdesc;

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof server_address);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htons(INADDR_ANY);
    server_address.sin_port = htons(MYPORT);
    
    printf("ip address: %s\n", inet_ntoa(server_address.sin_addr));
    printf("port: %d\n", (int) ntohs(server_address.sin_port));

    if ((listen_socket_fdesc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
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

    pthread_t thread;
    pthread_create(&thread, NULL, &t_receive, &listen_socket_fdesc);
    int on = 1;
    int input;
    int toport;

    printf("1 for test\n");
    while(on){
        scanf("%d", &input);
        if (input == 1){
            printf("enter port:\n");
            scanf("%d", &toport);
            send_to(toport, LOOPBACK_IP, "test msg");
        } else {
            on = 0;
        }
    }
    pthread_cancel(thread);
    pthread_join(thread, NULL);

    return 0;
}