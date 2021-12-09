#include <sys/types.h>
#include <sys/socket.h> 
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

//send() and rcv() system routines

int main(int argc,char **argv)
{
    char snd_msg[100];
    char rcv_msg[100];

    int socket_fdesc = socket(AF_INET, SOCK_STREAM, 0);
    printf("Socket status: %d\n", socket_fdesc);

    struct sockaddr_in server_address;
    //bzero(&server_address, sizeof(server_address));
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(6463);

    inet_pton(AF_INET, "127.0.0.1", &(server_address.sin_addr));
    int c = connect(socket_fdesc, (struct sockaddr *) &server_address, sizeof(server_address));

    printf("Connect status:%d\n", c);

    while(1){
        memset(&snd_msg, 0, sizeof(snd_msg));
        memset(&rcv_msg, 0, sizeof(rcv_msg));
        fgets(snd_msg, 100, stdin);
        write(socket_fdesc, snd_msg, strlen(snd_msg)+1);
        read(socket_fdesc, rcv_msg, 100);
        printf("Received msg:%s\n", rcv_msg);
    }


    return 0;
}