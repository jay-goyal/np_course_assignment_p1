#ifndef CMD_H
#define CMD_H

#include <stdbool.h>
#include <stddef.h>
typedef struct command
{
    char **cmd_arr;
    size_t size;
    int stdin_fd;
    int stdout_fd;
} command_t;

typedef struct command_list
{
    command_t *cmd_list;
    int *pipefds;
    size_t size;
    size_t pipe_count;
    bool is_trip;
    int ofd;
    int cfd;
    int ifd[3];
} command_list_t;

#endif // !CMD_H
