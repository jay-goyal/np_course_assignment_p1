#ifndef UDP_UTILS_H
#define UDP_UTILS_H

#include "types/cluster_data.h"
#include <arpa/inet.h>

#define MAX_UDP_PACKET 65507

void send_cluster_data_udp(cluster_data_t *data, int socket, in_addr_t dest_ip,
                           int port);
cluster_data_t recv_cluster_data_udp(int socket);

#endif // !UDP_UTILS_H
