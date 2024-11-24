#include <arpa/inet.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// PROJECT IMPORTS
#include "main.h"
#include "parse_command.h"
#include "spawn_proc.h"
#include "types/cmd.h"
#include "udp/command_data.h"
#include "utils/misc.h"
#include "utils/printfn.h"

void send_command(char *command, int udp_socket, in_addr_t dest_ip, int port)
{
    size_t cmd_len = strlen(command);

    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = dest_ip;
    dest_addr.sin_port = htons(port);

    for(size_t i = 0; i < cmd_len; i += DEFAULT_UDP_SIZE)
    {
        size_t chunk_size = (cmd_len - i > DEFAULT_UDP_SIZE) ? DEFAULT_UDP_SIZE
                                                             : cmd_len - i;
        if(sendto(udp_socket, command + i, chunk_size, 0,
                  (struct sockaddr *) &dest_addr, sizeof(dest_addr))
           < 0)
            exit_err_status("SENDTO FAILED WITH: ");
    }

    if(sendto(udp_socket, command, 0, 0, (struct sockaddr *) &dest_addr,
              sizeof(dest_addr))
       < 0)
        exit_err_status("SENDTO FAILED WITH: ");
}

void recv_command(int udp_socket, in_addr_t src_ip, int port)
{
    char *udp_buffer = malloc(sizeof(char) * (DEFAULT_UDP_SIZE + 1));
    size_t buf_size = DEFAULT_UDP_SIZE - 1;
    char *ptr = udp_buffer;

    struct sockaddr_in src_addr;
    src_addr.sin_family = AF_INET;
    src_addr.sin_addr.s_addr = src_ip;
    src_addr.sin_port = htons(port);

    socklen_t socklen = sizeof(src_addr);

    int n = 0;

    while(1)
    {
        n = recvfrom(udp_socket, ptr, DEFAULT_UDP_SIZE, 0,
                     (struct sockaddr *) &src_addr, &socklen);
        udp_buffer[buf_size - 1] = '\0';
        if(n == 0)
        {
            break;
        }
        udp_buffer = realloc(udp_buffer,
                             sizeof(char) * (buf_size + DEFAULT_UDP_SIZE));
        ptr += DEFAULT_UDP_SIZE;
        buf_size += DEFAULT_UDP_SIZE;
    }

    int fds[2];
    if(pipe(fds) != 0)
        exit_err_status("PIPE FAILED WITH: ");

    command_list_t *cmd_list
        = parse_command(udp_buffer, buf_size, STDIN_FILENO, fds[1]);
    size_t cmd_list_size = cmd_list->size;
    num_process += cmd_list_size;

    cmd_list->pipe_count += 2;
    cmd_list->pipefds = realloc(cmd_list->pipefds, sizeof(*cmd_list->pipefds)
                                                       * cmd_list->pipe_count);
    cmd_list->pipefds[cmd_list->pipe_count - 2] = fds[0];
    cmd_list->pipefds[cmd_list->pipe_count - 1] = fds[1];

    for(size_t i = 0; i < cmd_list_size; i++)
    {
        command_t *command = cmd_list->cmd_list + i;
        size_t cmd_size = command->size;

        spawn_proc(command, cmd_list->pipefds, cmd_list->pipe_count);
    }

    if(cmd_list->is_trip)
    {
        char buffer[PIPE_BUF_SIZE];
        ssize_t bytes_read;

        bzero(buffer, PIPE_BUF_SIZE);
        close(cmd_list->cfd);

        while((bytes_read = read(cmd_list->ofd, buffer, PIPE_BUF_SIZE)) > 0)
        {
            for(int i = 0; i < 3; i++)
                if(write(cmd_list->ifd[i], buffer, bytes_read) == -1)
                    exit_err_status("WRITE FAILED WITH: ");
            bzero(buffer, PIPE_BUF_SIZE);
        }
    }

    ssize_t bytes_read;

    char buf[PIPE_BUF_SIZE];
    bzero(buf, PIPE_BUF_SIZE);

    for(int i = 0; i < cmd_list->pipe_count; i++)
        if(cmd_list->pipefds[i] != fds[0])
            close(cmd_list->pipefds[i]);

    while((bytes_read = read(fds[0], buf, PIPE_BUF_SIZE)) > 0)
    {
        if(sendto(udp_socket, buf, bytes_read, 0,
                  (struct sockaddr *) &src_addr, sizeof(src_addr))
           < 0)
            exit_err_status("SENDTO FAILED WITH: ");
        bzero(buf, PIPE_BUF_SIZE);
    }

    if(sendto(udp_socket, buf, 0, 0, (struct sockaddr *) &src_addr,
              sizeof(src_addr))
       < 0)
        exit_err_status("SENDTO FAILED WITH: ");

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

    free(udp_buffer);
}

void recv_output(int udp_socket, in_addr_t src_ip, int port)
{
    char *buffer = malloc(sizeof(char) * (DEFAULT_UDP_SIZE + 1));

    struct sockaddr_in src_addr;
    src_addr.sin_family = AF_INET;
    src_addr.sin_addr.s_addr = src_ip;
    src_addr.sin_port = htons(port);

    socklen_t socklen = sizeof(src_addr);

    int n = 0;

    while(1)
    {
        n = recvfrom(udp_socket, buffer, DEFAULT_UDP_SIZE, 0,
                     (struct sockaddr *) &src_addr, &socklen);
        buffer[DEFAULT_UDP_SIZE] = '\0';
        if(n == 0)
        {
            break;
        }
        PRINTF_FG_WHITE("%s", buffer);
    }
    free(buffer);
}
