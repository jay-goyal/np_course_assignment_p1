#define _GNU_SOURCE
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

// PROJECT IMPORTS
#include "main.h"
#include "parse_command.h"
#include "signal_handlers.h"
#include "spawn_proc.h"
#include "types/cluster_data.h"
#include "types/cmd.h"
#include "udp/cluster_info.h"
#include "udp/command_data.h"
#include "utils/misc.h"
#include "utils/printfn.h"

unsigned int num_process = 0;
bool is_cluster = false;
cluster_data_t global_cluster_data;

int main(int argc, char *argv[])
{
    setup_sighandlers();
    bool to_exit = false;

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);

    if(flags == -1)
        exit_err_status("FNCTL ERROR: ");

    if(fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK) == -1)
        exit_err_status("FNCTL ERROR: ");

    struct sockaddr_in self_addr;
    socklen_t addr_len = sizeof(self_addr);
    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(udp_socket < 0)
        exit_err_status("FAILED TO CREATE UDP SOCKET:");

    memset(&self_addr, 0, sizeof(self_addr));
    self_addr.sin_family = AF_INET;
    self_addr.sin_addr.s_addr = INADDR_ANY;
    self_addr.sin_port = 0;

    if(bind(udp_socket, (struct sockaddr *) &self_addr, addr_len) < 0)
        exit_err_status("FAILED TO BIND UDP SOCKET:");

    if(getsockname(udp_socket, (struct sockaddr *) &self_addr, &addr_len) < 0)
        exit_err_status("FAILED TO GET UDP SOCKET INFO:");

    PRINTF_FG_MAGENTA("TERMINAL BOUND TO PORT: %d", ntohs(self_addr.sin_port));
    PRINTF_FG_WHITE("\n");

    while(!to_exit)
    {
        command_list_t *cmd_list = getcmdlist(udp_socket);
        size_t cmd_list_size = cmd_list->size;
        if(!cmd_list->is_cluster)
        {
            num_process += cmd_list_size;
            for(size_t i = 0; i < cmd_list_size; i++)
            {
                command_t *command = cmd_list->cmd_list + i;
                size_t cmd_size = command->size;

                if(strcmp(command->cmd_arr[0], "exit") == 0
                   || strcmp(command->cmd_arr[0], "exit\n") == 0)
                {
                    PRINTF_FG_GREEN("EXITING");
                    PRINTF_FG_WHITE("\n");
                    num_process -= 1;
                    to_exit = true;
                    break;
                }

                if(!is_cluster)
                    spawn_proc(command, cmd_list->pipefds,
                               cmd_list->pipe_count);
                else
                {
                    num_process -= cmd_list_size;
                    if(command->stdout_fd != 0)
                    {
                        send_command(
                            command->cmd_arr[0], udp_socket,
                            global_cluster_data
                                .ip_address_array[command->stdout_fd - 1],
                            global_cluster_data
                                .ports_array[command->stdout_fd - 1]);
                        if(global_cluster_data
                               .ports_array[command->stdout_fd - 1]
                           == ntohs(self_addr.sin_port))
                            recv_command(udp_socket, INADDR_ANY,
                                         ntohs(self_addr.sin_port));
                        recv_output(
                            udp_socket,
                            global_cluster_data
                                .ip_address_array[command->stdout_fd - 1],
                            global_cluster_data
                                .ports_array[command->stdout_fd - 1]);
                    }
                    else
                    {
                        for(int i = 0; i < global_cluster_data.addr_count; i++)
                            send_command(
                                command->cmd_arr[0], udp_socket,
                                global_cluster_data.ip_address_array[i],
                                global_cluster_data.ports_array[i]);
                        recv_command(udp_socket, INADDR_ANY,
                                     ntohs(self_addr.sin_port));
                        for(int i = 0; i < global_cluster_data.addr_count; i++)
                        {
                            PRINTF_FG_CYAN("-------OUTPUT OF NODE n%d-------",
                                           i);
                            PRINTF_FG_WHITE("\n");
                            recv_output(
                                udp_socket,
                                global_cluster_data.ip_address_array[i],
                                global_cluster_data.ports_array[i]);
                        }
                    }
                }
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
        }
        else
        {
            cluster_data_t cluster_data
                = { .addr_count = cmd_list->addr_count,
                    .ip_address_array = cmd_list->ip_address_array,
                    .ports_array = cmd_list->ports_array };
            for(int i = 0; i < cluster_data.addr_count; i++)
            {
                send_cluster_data_udp(&cluster_data, udp_socket,
                                      cluster_data.ip_address_array[i],
                                      cluster_data.ports_array[i]);
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
        free(cmd_list->ip_address_array);
        free(cmd_list->ports_array);
        free(cmd_list);

        while(num_process > 0)
            ;
    }

    return EXIT_SUCCESS;
}
