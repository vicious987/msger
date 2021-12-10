#ifndef P2P_H_
#define P2P_H_

#define LOOPBACK_IP "127.0.0.1"
#define LIMIT 10

int send_to(int receiver_port, char* receiver_ip, char* msg);

void receive(int server_fd);

void *t_receive(void *server_fd);

#endif
