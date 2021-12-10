#include <sys/types.h>
#include <sys/socket.h> 
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

int LIMIT = 10;
char* LOOPBACK_IP = "127.0.0.1";

int send_to(int receiver_port, char* receiver_ip, char* msg){
    int socket_fdesc;
    if ((socket_fdesc = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("Message failed on socket!\n");
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(receiver_port);
    inet_pton(AF_INET, receiver_ip, &(server_address.sin_addr));

    if (connect(socket_fdesc, (struct sockaddr *) &server_address, sizeof(server_address)) < 0){
        printf("Message failed on connect!\n");
        return -2;
    }
    write(socket_fdesc, msg, strlen(msg) + 1);
    close(socket_fdesc);
    printf("Message sent!\n");
    return 0;
}

void receive(int server_fd){ // replace select and fd sets with poll
    int socket;
    char msg[100];

    fd_set curr_fd_sockset, rdy_fd_sockset;
    FD_ZERO(&curr_fd_sockset);
    FD_ZERO(&rdy_fd_sockset);

    FD_SET(server_fd, &curr_fd_sockset);
    while(1) { // should it end somehow?
        rdy_fd_sockset = curr_fd_sockset;

        if (select(FD_SETSIZE, &rdy_fd_sockset, NULL, NULL, NULL) < 0){ // setsize +1 ?
            perror("select in receive failed");
            exit(1);
        }

        for (int fd = 0; fd < FD_SETSIZE; fd++){
            if (FD_ISSET(fd , &rdy_fd_sockset)){
                if (fd == server_fd){
                    if ((socket = accept(server_fd, (struct sockaddr*) NULL, NULL)) < 0){
                        perror("accept in receive failed");
                        exit(1);
                    }
                    FD_SET(socket, &curr_fd_sockset);
                } else {
                    memset(msg, 0, sizeof(msg));
                    read(socket, msg, 100); 
                    printf("%s\n", msg);
                    FD_CLR(fd, &curr_fd_sockset);
                }
            }
        }
    }
}

void *t_receive(void *server_fd){
    printf("Listening thread created!\n");
    receive(*(int *) server_fd);
}

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
    printf("port is: %d\n", (int) ntohs(server_address.sin_port));

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

    printf("1 for test");
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
