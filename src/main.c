#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// PROJECT IMPORTS
#include "main.h"
#include "parse_command.h"
#include "signal_handlers.h"
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
                continue;
            }

            pid_t pid = fork();
            if(pid == 0)
                execvp(command->cmd_arr[0], command->cmd_arr);
        }
        while(num_process > 0)
            ;
    }

    return EXIT_SUCCESS;
}
