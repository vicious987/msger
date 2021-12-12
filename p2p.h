#ifndef P2P_H_
#define P2P_H_

#define LOOPBACK_IP "127.0.0.1"
#define LIMIT 10
#define BUFFER_SIZE 1024

int send_to(int receiver_port, char* receiver_ip, char* msg);
int send_file_to(int receiver_port, char* receiver_ip);

void receive_file(int server_fd);

void * t_receive(void *server_fd);
void * t_receive_file(void *server_fd);
// POST REFACTOR

int create_sending_socket(int receiver_port, char* receiver_ip);
int create_listening_socket(int listening_port);
int send_str(int socket, char* msg);
int send_bytebuffer(int socket, unsigned char *buff, size_t buffsize);

void rcv_and_printstr(int socket, char* buffer, size_t buffsize);
void receive(int server_fd);
void* t_receive(void *server_fd);
#endif
