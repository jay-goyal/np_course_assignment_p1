#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

// PROJECT IMPORTS
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

void recv_output(int udp_socket, in_addr_t dest_ip, int port)
{
    char *buffer = malloc(sizeof(char) * (DEFAULT_UDP_SIZE + 1));

    struct sockaddr_in src_addr;
    src_addr.sin_family = AF_INET;
    src_addr.sin_addr.s_addr = dest_ip;
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
    PRINTF_FG_WHITE("\n");
    free(buffer);
}
