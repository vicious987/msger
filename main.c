
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
    int send_socket_fdesc;
    int receiver_port;
    char receiver_ip[15];
    memset(receiver_ip, 0, sizeof(receiver_ip));

    switch (argc){
        case 1: 
            my_port = DEFAULT_LISTENING_PORT;
            break;
        case 2:
            my_port = atoi(argv[1]);
            break;
        default:
            my_port = DEFAULT_LISTENING_PORT;
            break;
    };

/*
    if ((listen_socket_fdesc = create_listening_socket(my_port)) < 0){ //replace with while
        printf("Port %d is in use, please enter another listening port:");
        scanf("%d", &my_port);
    }
    */

    listen_socket_fdesc = create_listening_socket(my_port);
    printf("Using listening port %d...\n", my_port);


    pthread_t thread;
    pthread_create(&thread, NULL, &t_receive, &listen_socket_fdesc);


    printf("\nEnter receiver IP: ");
    scanf("%s", receiver_ip);
    printf("\nEnter receiver port: ");
    scanf("%d", &receiver_port);
    send_socket_fdesc = create_sending_socket(receiver_port, receiver_ip);


    int on = 1;
    char command[20];
    char* input;
    printf("To send a message, type !m <message>\n");
    printf("To send a file, type !f <filename>\n");
    printf("To exit, type !exit\n");
    //printf("To see manual, !help\n");
    while(on){
        scanf("%s", command);
        if (command[0] != '!' || strlen(command) < 2){
            printf("Command unrecognized, please try again\n");
            continue;
        }
        switch (command[1]){
            case 'm':
                send_control_char(send_socket_fdesc, 'm');
                //send_str(send_socket_fdesc, "test msg!");
                break;
            case 'e':
                on = 0;
                break;
            default:
                printf("Command unrecognized, please try again\n");
        }
    }

    close(listen_socket_fdesc);
    close(send_socket_fdesc);
    pthread_cancel(thread);
    pthread_join(thread, NULL);

    return 0;
}