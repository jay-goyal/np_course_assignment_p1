#include <arpa/inet.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// PROJECT IMPORTS
#include "parse_command.h"
#include "types/cmd.h"
#include "utils/misc.h"
#include "utils/printfn.h"
command_list_t *parse_command(char *cmd_str, int cmd_len)
{
    // Initilize command list
    command_list_t *cmds = (command_list_t *) malloc(sizeof(command_list_t));
    cmds->cmd_list = (command_t *) malloc(sizeof(command_t));
    cmds->size = 1;
    cmds->pipefds = NULL;
    cmds->pipe_count = 0;
    cmds->is_trip = false;
    cmds->is_cluster = false;
    cmds->ip_address_array = NULL;
    cmds->addr_count = 0;
    cmds->ports_array = NULL;
    cmds->ofd = 0;
    for(int i = 0; i < 3; i++)
        cmds->ifd[i] = 0;

    // Initiliaze first command
    command_t *cmd = &cmds->cmd_list[0];
    cmd->cmd_arr = (char **) malloc(sizeof(char *));
    cmd->size = 1;
    cmd->stdin_fd = STDIN_FILENO;
    cmd->stdout_fd = STDOUT_FILENO;

    // Initialize first command string
    size_t cmd_size = BASE_CMD_SIZE;
    size_t cmd_idx = 0;
    char *curr_string = (char *) malloc(cmd_size * sizeof(char));
    cmd->cmd_arr[0] = curr_string;
    bool is_str_start = true;
    size_t ifd_idx = 0;

    for(int i = 0; i < cmd_len; i++)
    {
        char c = cmd_str[i];

        if(c == '|')
        {
            if(cmd_idx != 0)
            {
                // Add \0 at end of current string
                curr_string
                    = realloc(curr_string, (cmd_idx + 1) * sizeof(char));
                curr_string[cmd_idx] = '\0';
                cmd->cmd_arr[cmd->size - 1] = curr_string;

                // Add another element to cmd_arr array of current command
                cmd->cmd_arr
                    = realloc(cmd->cmd_arr, (cmd->size + 1) * sizeof(char *));
                cmd->cmd_arr[cmd->size++] = NULL;
            }
            else
            {
                free(curr_string);
                cmd->cmd_arr[cmd->size - 1] = NULL;
            }

            int fds[2];
            if(pipe(fds) != 0)
                exit_err_status("FAILED TO CREATE PIPE:");

            // Add another command to cmd_list
            cmds->cmd_list = (command_t *) realloc(
                cmds->cmd_list, (cmds->size + 1) * sizeof(command_t));

            // Add pipes to pipefds
            cmds->pipefds = (int *) realloc(
                cmds->pipefds, (cmds->pipe_count + 2) * sizeof(int));
            cmds->pipefds[cmds->pipe_count++] = fds[0];
            cmds->pipefds[cmds->pipe_count++] = fds[1];

            cmds->cmd_list[cmds->size - 1].stdout_fd = fds[1];
            cmd = &cmds->cmd_list[cmds->size++];

            // Set command struct correctly
            if(cmd_str[i + 1] == '|' && cmd_str[i + 2] == '|')
            {
                cmds->ofd = fds[0];
                cmds->cfd = fds[1];
                cmds->is_trip = true;

                if(pipe(fds) != 0)
                    exit_err_status("FAILED TO CREATE PIPE:");

                // Add pipes to pipefds
                cmds->pipefds = (int *) realloc(
                    cmds->pipefds, (cmds->pipe_count + 2) * sizeof(int));
                cmds->pipefds[cmds->pipe_count++] = fds[0];
                cmds->pipefds[cmds->pipe_count++] = fds[1];
                cmds->ifd[ifd_idx++] = fds[1];
                i += 2;
            }
            cmd->stdin_fd = fds[0];
            cmd->stdout_fd = STDOUT_FILENO;
            cmd->cmd_arr = (char **) malloc(sizeof(char *));
            cmd->size = 1;

            // Create command array
            cmd_size = BASE_CMD_SIZE;
            cmd_idx = 0;
            curr_string = (char *) malloc(cmd_size * sizeof(char));
            cmd->cmd_arr[0] = curr_string;
            is_str_start = true;
            continue;
        }

        if(c == '\n')
        {
            // Add \0 at end of current string
            curr_string = realloc(curr_string, (cmd_idx + 1) * sizeof(char));
            curr_string[cmd_idx] = '\0';

            // Add another element to cmd_arr array of current command
            if(cmds->is_cluster)
            {
                cmds->addr_count++;
                cmds->ip_address_array = realloc(
                    cmds->ip_address_array,
                    sizeof(*cmds->ip_address_array) * cmds->addr_count);
                cmds->ports_array
                    = realloc(cmds->ports_array,
                              sizeof(*cmds->ports_array) * cmds->addr_count);
                cmds->ip_address_array[cmds->addr_count - 1]
                    = inet_addr(curr_string);
                char *ip = strtok(curr_string, "::");
                char *port = strtok(NULL, "::");
                if(!ip || !port)
                {
                    PRINTF_FG_RED("INVALID STRING PASSED IN IP");
                    PRINTF_FG_WHITE("\n");
                    exit(EXIT_FAILURE);
                }
                cmds->ip_address_array[cmds->addr_count - 1] = inet_addr(ip);
                cmds->ports_array[cmds->addr_count - 1] = atoi(port);
                free(curr_string);
            }
            else
            {
                cmd->cmd_arr[cmd->size - 1] = curr_string;
                cmd->cmd_arr
                    = realloc(cmd->cmd_arr, (cmd->size + 1) * sizeof(char *));
                cmd->cmd_arr[cmd->size++] = NULL;
            }
            break;
        }

        if(c == ' ')
        {
            if(!is_str_start)
            {
                // Add \0 at end of current string
                curr_string
                    = realloc(curr_string, (cmd_idx + 1) * sizeof(char));
                curr_string[cmd_idx] = '\0';

                if(cmd->size == 1 && strcmp(curr_string, "cluster") == 0)
                {
                    cmds->is_cluster = true;
                    cmds->addr_count = 0;
                    free(curr_string);
                }

                else if(cmds->is_cluster)
                {
                    cmds->addr_count++;
                    cmds->ip_address_array = realloc(
                        cmds->ip_address_array,
                        sizeof(*cmds->ip_address_array) * cmds->addr_count);
                    cmds->ports_array
                        = realloc(cmds->ports_array, sizeof(*cmds->ports_array)
                                                         * cmds->addr_count);
                    cmds->ip_address_array[cmds->addr_count - 1]
                        = inet_addr(curr_string);
                    char *ip = strtok(curr_string, "::");
                    char *port = strtok(NULL, "::");
                    cmds->ip_address_array[cmds->addr_count - 1]
                        = inet_addr(ip);
                    cmds->ports_array[cmds->addr_count - 1] = atoi(port);
                    free(curr_string);
                }
                else
                {

                    // Add another element to cmd_arr array of current command
                    cmd->cmd_arr[cmd->size - 1] = curr_string;
                    cmd->cmd_arr = realloc(cmd->cmd_arr,
                                           (cmd->size + 1) * sizeof(char *));
                }

                // Reset the string
                cmd_size = BASE_CMD_SIZE;
                curr_string = (char *) malloc(cmd_size * sizeof(char));
                cmd_idx = 0;
                is_str_start = true;
                if(!cmds->is_cluster)
                    cmd->cmd_arr[cmd->size++] = curr_string;
            }
            continue;
        }

        if(c == ',' && cmds->is_trip)
        {
            if(cmd_idx != 0)
            {
                // Add \0 at end of current string
                curr_string
                    = realloc(curr_string, (cmd_idx + 1) * sizeof(char));
                curr_string[cmd_idx] = '\0';
                cmd->cmd_arr[cmd->size - 1] = curr_string;

                // Add another element to cmd_arr array of current command
                cmd->cmd_arr
                    = realloc(cmd->cmd_arr, (cmd->size + 1) * sizeof(char *));
                cmd->cmd_arr[cmd->size++] = NULL;
            }
            else
            {
                free(curr_string);
                cmd->cmd_arr[cmd->size - 1] = NULL;
            }

            int fds[2];
            if(pipe(fds) != 0)
                exit_err_status("FAILED TO CREATE PIPE:");

            // Add another command to cmd_list
            cmds->cmd_list = (command_t *) realloc(
                cmds->cmd_list, (cmds->size + 1) * sizeof(command_t));

            // Add pipes to pipefds
            cmds->pipefds = (int *) realloc(
                cmds->pipefds, (cmds->pipe_count + 2) * sizeof(int));
            cmds->pipefds[cmds->pipe_count++] = fds[0];
            cmds->pipefds[cmds->pipe_count++] = fds[1];

            cmd = &cmds->cmd_list[cmds->size++];
            cmds->ifd[ifd_idx++] = fds[1];

            // Set command struct correctly
            cmd->stdin_fd = fds[0];
            cmd->stdout_fd = STDOUT_FILENO;
            cmd->cmd_arr = (char **) malloc(sizeof(char *));
            cmd->size = 1;

            // Create command array
            cmd_size = BASE_CMD_SIZE;
            cmd_idx = 0;
            curr_string = (char *) malloc(cmd_size * sizeof(char));
            cmd->cmd_arr[0] = curr_string;
            is_str_start = true;
            continue;
        }

        // Increase size of string if command is too long
        if(cmd_idx >= cmd_size)
        {
            cmd_size *= 2;
            curr_string = realloc(curr_string, cmd_size * sizeof(char));
            cmd->cmd_arr[cmd->size - 1] = curr_string;
        }

        curr_string[cmd_idx++] = c;
        is_str_start = false;
    }

    return cmds;
}

command_list_t *getcmdlist(void)
{
    char *lineptr = NULL;
    size_t n = 0;
    size_t buf_size = 0;
    PRINTF_FG_WHITE("$ ");
    fflush(stdout);

    n = getline(&lineptr, &buf_size, stdin);
    command_list_t *cmd_list = parse_command(lineptr, n);
    free(lineptr);
    return cmd_list;
}
