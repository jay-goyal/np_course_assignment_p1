#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// PROJECT IMPORTS
#include "spawn_proc.h"
#include "utils/misc.h"

void spawn_proc(command_t *cmd, int *pipefds, size_t num_pipes)
{
    pid_t pid = fork();
    if(pid != 0)
        return;

    if(cmd->stdin_fd != STDIN_FILENO)
        if(dup2(cmd->stdin_fd, STDIN_FILENO) == -1)
            exit_err_status("STDIN DUP2 FAILED WITH:");

    if(cmd->stdout_fd != STDOUT_FILENO)
        if(dup2(cmd->stdout_fd, STDOUT_FILENO) == -1)
            exit_err_status("STDOUT DUP2 FAILED WITH:");

    for(int i = 0; i < num_pipes; i++)
        close(pipefds[i]);

    if(execvp(cmd->cmd_arr[0], cmd->cmd_arr) == -1)
        exit_err_status("EXEC FAILED WITH: ");
}
