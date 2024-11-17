#ifndef CMD_H
#define CMD_H

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
} command_list_t;

#endif // !CMD_H
