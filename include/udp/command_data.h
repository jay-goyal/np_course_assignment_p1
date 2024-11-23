#ifndef COMMAND_DATA_H
#define COMMAND_DATA_H

#include <arpa/inet.h>

#define DEFAULT_UDP_SIZE 1024

void send_command(char *command, int udp_socket, in_addr_t dest_ip, int port);
void recv_command(int udp_socket, in_addr_t src_ip, int port);
void recv_output(int udp_socket, in_addr_t dest_ip, int port);

#endif // !COMMAND_DATA_H
