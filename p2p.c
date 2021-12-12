#include <sys/types.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "p2p.h"

int send_file_to(int receiver_port, char* receiver_ip){
    int socket_fdesc;
    if ((socket_fdesc = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("sendfile failed on socket!\n");
        return -1;
    }
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(receiver_port);
    inet_pton(AF_INET, receiver_ip, &(server_address.sin_addr));

    if (connect(socket_fdesc, (struct sockaddr *) &server_address, sizeof(server_address)) < 0){
        printf("sendfile failed on connect!\n");
        return -2;
    } 

    FILE *file= fopen("in.jpg", "rb");
    if (file == NULL){
        printf("sendfile Failed to open file\n");
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);
    if (file_size == EOF) {
        return -1;
    }

    //informing about file size
    long expected_filesize = htonl(file_size);
    int bytes_send = send(socket_fdesc, &expected_filesize, sizeof(expected_filesize), 0); // different sizes of longs on architectures??
    if (bytes_send != sizeof(expected_filesize)){
        printf("sendfile: issue on sending expected size\n");
        return -1;
    }

    //sending file
    unsigned char buffer[BUFFER_SIZE]; // typedef unsigned char to BYTE?
    size_t bytes_read = 0;
    size_t bytes_sent = 0;
    size_t chunk_size = 0;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        chunk_size = bytes_read < sizeof(buffer) ? bytes_read : sizeof(buffer);
        bytes_sent = send(socket_fdesc, buffer, chunk_size, 0);  //assume that send is gonna send in single go
        if (bytes_sent < chunk_size){
            printf("sendfile: chunk too big for send!\n");
            return -1;
        }
    }

    fclose(file);
    close(socket_fdesc);
    printf("File send!\n");
    return 1;
}


int send_to(int receiver_port, char* receiver_ip, char* msg){
    int socket_fdesc;

    if ((socket_fdesc = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("Message failed on socket!\n");
        return -1;
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

    int total_bytes_sent = 0;

    //printf("start sending msg, bytesize: %ld", sizeof(msg));
    while (total_bytes_sent < sizeof(msg)){ //what if send errors (returns <0)
        total_bytes_sent += send(socket_fdesc, msg + total_bytes_sent, strlen(msg + total_bytes_sent), 0);
        //printf("chunk sent, %d sent so far\n", total_bytes_sent);
    }
    printf("Message sent!\n");

    close(socket_fdesc);
    return 0;
}

void receive(int server_fd){ // replace select and fd sets with poll
    int socket;
    char msg[BUFFER_SIZE];

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

                    //fcntl(socket, F_SETFL, O_NONBLOCK); // set socket to non-blocking // do i need this?
                    FD_SET(socket, &curr_fd_sockset);
                } else {
                    memset(msg, 0, sizeof(msg));
                    recv(socket, msg, sizeof(msg), 0);
                    //read(socket, msg, 100); 
                    printf("%s\n", msg);
                    FD_CLR(fd, &curr_fd_sockset);
                }
            }
        }
    }
}

void receive_file(int server_fd){ // replace select and fd sets with poll
    int socket;

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

        unsigned char buffer[BUFFER_SIZE];
        memset(buffer, 0, sizeof(buffer));
        for (int fd = 0; fd < FD_SETSIZE; fd++){
            if (FD_ISSET(fd , &rdy_fd_sockset)){
                if (fd == server_fd){
                    if ((socket = accept(server_fd, (struct sockaddr*) NULL, NULL)) < 0){
                        perror("accept in receive failed");
                        exit(1);
                    }

                    //fcntl(socket, F_SETFL, O_NONBLOCK); // set socket to non-blocking // do i need this?
                    FD_SET(socket, &curr_fd_sockset);
                } else {
                    FILE *f = fopen("out.jpg", "wb");
                    long fsize = 0;
                    size_t bytes_received = 0;
                    size_t written = 0;
                    size_t written_so_far = 0;
                    recv(socket, &fsize, sizeof(long), 0);
                    fsize = ntohl(fsize);
                    if (fsize > 0){
                        //read to buffer
                        bytes_received  = recv(socket, &buffer, sizeof(buffer), 0);
                        //write from buffer to file
                        written_so_far = 0;
                        while (fsize > 0) { 
                            written = fwrite(buffer + written_so_far, 1, bytes_received, f);
                            if (written < 1){
                                return;
                            }
                            written_so_far += written;
                            fsize -= written;
                        }
                    fclose(f);
                    }

                    //recv(socket, msg, sizeof(msg), 0);
                    //printf("%s\n", msg);
                    FD_CLR(fd, &curr_fd_sockset);
                }
            }
        }
    }
}

void * t_receive(void *server_fd){
    printf("Listening thread created!\n");
    receive(*(int *) server_fd);
    return NULL;
}

void * t_receive_file(void *server_fd){
    printf("Listening thread created!\n");
    receive(*(int *) server_fd);
    return NULL;
}