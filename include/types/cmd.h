#ifndef CMD_H
#define CMD_H

#include <stddef.h>
typedef struct command
{
    char **cmd_arr;
    size_t size;
} command_t;

typedef struct command_list
{
    command_t *cmd_list;
    size_t size;
} command_list_t;

#endif // !CMD_H
