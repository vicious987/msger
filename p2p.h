#ifndef P2P_H_
#define P2P_H_

#define LOOPBACK_IP "127.0.0.1"
#define LIMIT 10
#define BUFFER_SIZE 1024
#define DEFAULT_LISTENING_PORT 8080

//void * t_receive(void *server_fd);
//void * t_receive_file(void *server_fd);
// POST REFACTOR

int create_sending_socket(int receiver_port, char* receiver_ip);
int create_listening_socket(int listening_port);

int send_str(int socket, char* msg);
int send_file(int socket, char* filename);

void* t_receive(void *server_fd);
void receive(int server_fd);
int rcv_and_printstr(int socket, char* buffer, size_t buffsize);
int rcv_and_save(int socket, char* filename, unsigned char* byte_buffer, size_t buffer_size);

ssize_t rcv_bytebuffer(int socket, unsigned char* buff, size_t buffsize);
int send_bytebuffer(int socket, unsigned char *buff, size_t buffsize);
int send_control_char(int socket, char c);

void receive2(int server_fd);
int send_test(int receiver_port, char* receiver_ip);
int send_to(int receiver_port, char* receiver_ip, char* msg);

int send_file_to(int receiver_port, char* receiver_ip, char* filename);



#endif
