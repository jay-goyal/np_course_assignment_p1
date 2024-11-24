#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// PROJECT IMPORTS
#include "utils/misc.h"

void exit_err_status(char *fmt) {
    perror(fmt);
    exit(-1);
}

char *in_addr_to_str(in_addr_t addr) {
    struct in_addr ip_struct;
    static char ip_str[INET_ADDRSTRLEN];

    ip_struct.s_addr = addr;

    if (inet_ntop(AF_INET, &ip_struct, ip_str, sizeof(ip_str)) == NULL)
        return NULL;

    return ip_str;
}
