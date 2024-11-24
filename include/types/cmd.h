#ifndef CMD_H
#define CMD_H

#include <arpa/inet.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct command {
    char **cmd_arr;
    size_t size;
    int stdin_fd;
    int stdout_fd;
} command_t;

typedef struct command_list {
    command_t *cmd_list;
    int *pipefds;
    size_t size;
    size_t pipe_count;
    bool is_trip;
    int ofd;
    int cfd;
    int ifd[3];
    bool is_cluster;
    size_t addr_count;
    in_addr_t *ip_address_array;
    int *ports_array;
} command_list_t;

#endif  // !CMD_H
