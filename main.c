
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "p2p.h"


int main(int argc, char **argv) {
    int my_port = 0;
    int listen_socket_fdesc;
    //int send_socket_fdesc;
    int receiver_port;
    char receiver_ip[15];

    my_port = atoi(argv[1]);
    receiver_port = atoi(argv[2]);

    listen_socket_fdesc = create_listening_socket(my_port);
    printf("Using listening port %d...\n", my_port);


    pthread_t thread;
    pthread_create(&thread, NULL, &t_receive, &listen_socket_fdesc);

    printf("To send a message, type !m <message>\n");
    printf("To send a file, type !f <filename>\n");
    printf("To exit, type !exit\n");

    int on = 1;
    int command = 0;
    //char input_buffer[BUFFER_SIZE];
    while(on){
        scanf("%d", &command);
        switch (command){
            case 1: {
                send_to(receiver_port, receiver_ip, "test message"); 
                break;
            }
            default : {
                on = 0;
                break;
            }
        }
    }

    close(listen_socket_fdesc);
    //close(send_socket_fdesc);
    pthread_cancel(thread);
    pthread_join(thread, NULL);

    return 0;
}