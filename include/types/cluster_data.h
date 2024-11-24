#ifndef CLUSTER_DATA_H
#define CLUSTER_DATA_H

#include <arpa/inet.h>
#include <stddef.h>

typedef struct {
    size_t addr_count;
    in_addr_t *ip_address_array;
    int *ports_array;
} cluster_data_t;

#endif  // !CLUSTER_DATA_H
