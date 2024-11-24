#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

// PROJECT IMPORTS
#include "types/cluster_data.h"
#include "udp/cluster_info.h"
#include "utils/misc.h"

void send_cluster_data_udp(cluster_data_t *data, int sock, in_addr_t dest_ip,
                           int dest_port) {
    ssize_t buffer_size = sizeof(data->addr_count) +
                          (sizeof(*data->ip_address_array) * data->addr_count) +
                          (sizeof(*data->ports_array) * data->addr_count);
    void *buffer = malloc(buffer_size);

    void *ptr = buffer;
    memcpy(ptr, &data->addr_count, sizeof(data->addr_count));
    ptr += sizeof(data->addr_count);
    memcpy(ptr, data->ip_address_array,
           data->addr_count * sizeof(*data->ip_address_array));
    ptr += data->addr_count * sizeof(*data->ip_address_array);
    memcpy(ptr, data->ports_array,
           data->addr_count * sizeof(*data->ports_array));

    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(dest_port);
    dest_addr.sin_addr.s_addr = dest_ip;

    ssize_t send_bytes =
        sendto(sock, buffer, buffer_size, 0, (struct sockaddr *)&dest_addr,
               sizeof(dest_addr));

    if (send_bytes < 0) exit_err_status("Failed to send data");

    free(buffer);
}

cluster_data_t recv_cluster_data_udp(int socket) {
    cluster_data_t data;
    void *buf = malloc(sizeof(size_t));
    ssize_t recv_bytes =
        recvfrom(socket, buf, sizeof(size_t), MSG_PEEK, NULL, NULL);

    data.addr_count = *((size_t *)buf);
    data.ip_address_array =
        malloc(sizeof(*data.ip_address_array) * data.addr_count);
    data.ports_array = malloc(sizeof(*data.ports_array) * data.addr_count);
    ssize_t buf_size = sizeof(data.addr_count) +
                       (sizeof(*data.ip_address_array) * data.addr_count) +
                       (sizeof(*data.ports_array) * data.addr_count);
    buf = realloc(buf, buf_size);

    recv_bytes = recvfrom(socket, buf, buf_size, 0, NULL, NULL);
    void *ptr = buf;
    memcpy(&data.addr_count, ptr, sizeof(data.addr_count));
    ptr += sizeof(data.addr_count);
    memcpy(data.ip_address_array, ptr,
           sizeof(*data.ip_address_array) * data.addr_count);
    ptr += sizeof(*data.ip_address_array) * data.addr_count;
    memcpy(data.ports_array, ptr, sizeof(*data.ports_array) * data.addr_count);

    free(buf);
    return data;
}
