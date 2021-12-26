#include <sys/types.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "p2p.h"

void * t_receive(void *server_fd){
    receive(*(int *) server_fd);
    return NULL;
}

int create_sending_socket(int receiver_port, char* receiver_ip){
    int socket_fdesc;

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(receiver_port);
    inet_pton(AF_INET, receiver_ip, &(server_address.sin_addr));

    if ((socket_fdesc = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        return -1;
    }

    if (connect(socket_fdesc, (struct sockaddr *) &server_address, sizeof(server_address)) < 0){
        return -1;
    }
    return socket_fdesc;
}

int create_listening_socket(int listening_port) {
    int listen_socket_fdesc;

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof server_address);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htons(INADDR_ANY);
    server_address.sin_port = htons(listening_port);
    
    printf("Creating listening socket on:\n");
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
    return listen_socket_fdesc;
}

int send_str(int socket, char* msg_str){
    int total_chars_sent = 0;
    while (total_chars_sent < sizeof(msg_str)) { //what if send errors (returns < 0)
        total_chars_sent += send(socket, msg_str + total_chars_sent, strlen(msg_str + total_chars_sent), 0);
    }
    printf("Message of length %d sent!\n", total_chars_sent);
    return 0;
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


    send(socket_fdesc, msg, strlen(msg), 0);
    /*
    int total_bytes_sent = 0;
    while (total_bytes_sent < sizeof(msg)){ //what if send errors (returns <0)
        total_bytes_sent += send(socket_fdesc, msg + total_bytes_sent, strlen(msg + total_bytes_sent), 0);
    }
    */

    printf("Message sent!\n");
    close(socket_fdesc);
    return 0;
}

void receive(int server_fd){ // replace select and fd sets with poll
    int socket;
    char msg_buffer[BUFFER_SIZE];
    char msg[100];
    memset(msg, 0, sizeof(msg));
    //unsigned char byte_buffer[BUFFER_SIZE];
    //char control_char;
    //int cc_rcv = 0;
    //int r = 0;


    fd_set active_fd_set, read_fd_set;
    FD_ZERO(&active_fd_set);
    FD_SET(server_fd, &active_fd_set);
    while(1) { // should it end somehow?
        read_fd_set = active_fd_set;

        if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0){
            exit(1);
        } 

        for (int fd = 0; fd < FD_SETSIZE; fd++) {
            if (FD_ISSET(fd , &read_fd_set)) {
                if (fd == server_fd) {
                    if ((socket = accept(server_fd, (struct sockaddr*) NULL, NULL)) < 0){
                        perror("accept in receive failed");
                        exit(1);
                    }
                    fcntl(socket, F_SETFL, O_NONBLOCK); // set socket to non-blocking // do i need this?
                    FD_SET (socket, &active_fd_set);
                } else {
                    memset(msg_buffer, 0, sizeof(msg_buffer));
                    //recv(fd, msg_buffer, sizeof(msg_buffer), 0);
                    //control_char = msg_buffer[0];
                    rcv_and_printstr(fd, msg_buffer, sizeof(msg_buffer));
                    /*
                    switch (control_char){
                        case 'm': {
                            rcv_and_printstr(fd, msg_buffer, sizeof(msg_buffer));
                            break;
                        }
                        case 'f': {
                            rcv_and_save(fd, "out.jpg", byte_buffer, sizeof(byte_buffer));
                            break;
                        }
                    }
                    */
                    FD_CLR (fd, &active_fd_set);
                }
            }
        }
    }
}


int rcv_and_printstr(int socket, char* buffer, size_t buffsize){
    memset(buffer, 0, buffsize);
    int i = recv(socket, buffer, buffsize, 0);
    printf("Received message: %s\n", buffer);
    return i;
}

int send_bytebuffer(int socket, unsigned char* buff, size_t buffsize) {
    for (int i = 0, bytes_sent = 0; i < buffsize; i += bytes_sent) {
        bytes_sent = send(socket, buff + i, buffsize - i, 0);
    }
    return 0;
}

ssize_t rcv_bytebuffer(int socket, unsigned char* buff, size_t buffsize){
    printf("\nrcv bytebuffer\n");
    ssize_t bytes_read = 0;
    //int total_bytes_read = 0;
    int i = 0;
    while(i < buffsize){
        bytes_read = recv(socket, buff + i, buffsize, 0);
        printf("%ld\t", bytes_read);
        if (bytes_read < 0){
            return -1;
        }
        if (bytes_read == 0){ //!
            //total_bytes_read = i;
            break;
        }
        i += bytes_read;
    }
    printf("\nrcv bytebuffer end\n");
    return i;
}


int rcv_and_save(int socket, char* filename, unsigned char* byte_buffer, size_t buffer_size){
    size_t bytes_written = 0;
    FILE* f = fopen(filename, "wb");
    long filesize = 0;
    long received_bytecount = 0;
    long total_bytecount = 0;
    int expected_buffer_size = 0;

    recv(socket, &filesize, sizeof(long), 0);
    filesize = ntohl(filesize);
    printf("Total file size expected: %ld\n", filesize);

    while (total_bytecount < filesize) {
        //get buffer
        expected_buffer_size = filesize - total_bytecount < buffer_size ? filesize - total_bytecount : buffer_size;
        received_bytecount = rcv_bytebuffer(socket, byte_buffer, expected_buffer_size);
        total_bytecount += received_bytecount;
        printf("received batch : %ld received total: %ld\n", received_bytecount, total_bytecount);

        //write buffer to file
        int i = 0;
        while (i < received_bytecount) {
            bytes_written = fwrite(byte_buffer + i, 1, buffer_size - i, f);
            if (bytes_written < 0){
                return -1;
            }
            i += bytes_written;
        }
    }
    printf("receiving done\n");
    fclose(f);
    return 0;
}

int send_file_to(int receiver_port, char* receiver_ip, char* filename){
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


    FILE* file = fopen(filename, "rb");
    long file_size;

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file);

    printf("Sending file of size: %ld", file_size);
    //informing about file size
    long expected_filesize = htonl(file_size);
    int bytes_send = send(socket_fdesc, &expected_filesize, sizeof(expected_filesize), 0); // different sizes of longs on architectures??
    if (bytes_send != sizeof(expected_filesize)){
        printf("sendfile: issue on sending expected size\n");
        return -1;
    }

    //sending file
    unsigned char buffer[BUFFER_SIZE]; // typedef unsigned char to BYTE?
    memset(buffer, 0, sizeof(buffer));
    size_t bytes_read = 0;
    size_t chunk_size = 0;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        chunk_size = bytes_read < sizeof(buffer) ? bytes_read : sizeof(buffer);
        send_bytebuffer(socket_fdesc, buffer, chunk_size);
    }

    close(socket_fdesc);
    fclose(file);
    printf("File send!\n");
    return 1;
}

int send_file(int socket, char* filename){
    FILE* file = fopen(filename, "rb");
    long file_size;

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file);

    printf("Sending file of size: %ld", file_size);

    //informing about file size
    long expected_filesize = htonl(file_size);
    int bytes_send = send(socket, &expected_filesize, sizeof(expected_filesize), 0); // different sizes of longs on architectures??
    if (bytes_send != sizeof(expected_filesize)){
        printf("sendfile: issue on sending expected size\n");
        return -1;
    }

    //sending file
    unsigned char buffer[BUFFER_SIZE]; // typedef unsigned char to BYTE?
    memset(buffer, 0, sizeof(buffer));
    size_t bytes_read = 0;
    size_t chunk_size = 0;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        chunk_size = bytes_read < sizeof(buffer) ? bytes_read : sizeof(buffer);
        send_bytebuffer(socket, buffer, chunk_size);
    }

    fclose(file);
    printf("File send!\n");
    return 1;

}