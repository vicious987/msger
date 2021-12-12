
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "p2p.h"


int main(int argc,char **argv) {
    int MY_PORT;
    int to_port;

    printf("Enter listening port: ");
    scanf("%d", &MY_PORT);
    printf("\n");

    int listen_socket_fdesc = create_listening_socket(MY_PORT);
    pthread_t thread;
    pthread_create(&thread, NULL, &t_receive, &listen_socket_fdesc);

    printf("Enter receiving port:\n");
    scanf("%d", &to_port);
    printf("\n");
    int send_socket_fdesc = create_sending_socket(to_port, LOOPBACK_IP);


    int on = 1;
    int input;
    printf("Type 1 to send a test message\n");
    while(on){
        scanf("%d", &input);
        if (input == 1){
            send_str(send_socket_fdesc, "test msg!");
        } else {
            on = 0;
        }
    }
    pthread_cancel(thread);
    pthread_join(thread, NULL);

    return 0;
}