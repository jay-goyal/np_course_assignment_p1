#include "utils/misc.h"
#include <strings.h>
#define _GNU_SOURCE
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// PROJECT IMPORTS
#include "main.h"
#include "parse_command.h"
#include "signal_handlers.h"
#include "spawn_proc.h"
#include "types/cmd.h"
#include "utils/printfn.h"

unsigned int num_process = 0;

int main(int argc, char *argv[])
{
    setup_sighandlers();
    bool to_exit = false;

    while(!to_exit)
    {
        command_list_t *cmd_list = getcmdlist();
        size_t cmd_list_size = cmd_list->size;
        PRINTF_FG_CYAN("Executing %zu commands", cmd_list_size);
        PRINTF_FG_WHITE("\n");
        num_process += cmd_list_size;
        for(size_t i = 0; i < cmd_list_size; i++)
        {
            command_t *command = cmd_list->cmd_list + i;
            size_t cmd_size = command->size;

            if(strcmp(command->cmd_arr[0], "exit") == 0)
            {
                PRINTF_FG_GREEN("EXITING");
                PRINTF_FG_WHITE("\n");
                num_process -= 1;
                to_exit = true;
                break;
            }

            spawn_proc(command, cmd_list->pipefds, cmd_list->pipe_count);
        }

        if(cmd_list->is_trip)
        {
            char buffer[PIPE_BUF_SIZE];
            ssize_t bytes_read;

            bzero(buffer, PIPE_BUF_SIZE);
            close(cmd_list->cfd);

            while((bytes_read = read(cmd_list->ofd, buffer, PIPE_BUF_SIZE))
                  > 0)
            {
                for(int i = 0; i < 3; i++)
                    if(write(cmd_list->ifd[i], buffer, bytes_read) == -1)
                        exit_err_status("WRITE FAILED WITH: ");
                bzero(buffer, PIPE_BUF_SIZE);
            }
        }

        for(int i = 0; i < cmd_list->pipe_count; i++)
            close(cmd_list->pipefds[i]);

        for(size_t i = 0; i < cmd_list_size; i++)
        {
            command_t *cmd = cmd_list->cmd_list + i;
            for(size_t i = 0; i < cmd->size - 1; i++)
                free(cmd->cmd_arr[i]);
            free(cmd->cmd_arr);
        }

        free(cmd_list->pipefds);
        free(cmd_list);

        while(num_process > 0)
            ;
    }

    return EXIT_SUCCESS;
}
