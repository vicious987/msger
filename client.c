#include <sys/types.h>
#include <sys/socket.h> 
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc,char **argv)
{
    char snd_msg[100];
    char rcv_msg[100];

    int socket_fdesc = socket(AF_INET, SOCK_STREAM, 0);
    printf("Socket status: %d\n", socket_fdesc);

    struct sockaddr_in server_address;
    //bzero(&server_address, sizeof(server_address));
    memset(&server_address, 0, sizeof(server_adress));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(6463);

    inet_pton(AF_INET, "127.0.0.1", &(server_address.sin_addr));
    int c = connect(socket_fdesc, (struct sockaddr *) &server_address, sizeof(server_address));

    printf("Connect status:%d\n", c);

    while(1){
        bzero(snd_msg, 100);
        bzero(rcv_msg, 100);
        //printf("yay\n");
        fgets(snd_msg, 100, stdin);
        //printf("yay\n");
        write(socket_fdesc, snd_msg, strlen(snd_msg)+1);
        //printf("yay\n");
        read(socket_fdesc, rcv_msg, 100);
        printf("Received msg:%s\n", rcv_msg);
    }


    return 0;
}