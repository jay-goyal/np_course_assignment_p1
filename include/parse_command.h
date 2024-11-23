#ifndef PARSE_COMMANDS_H
#define PARSE_COMMANDS_H

#include "types/cmd.h"

#define BASE_CMD_SIZE 128

command_list_t *getcmdlist(int udp_socket);
command_list_t *parse_command(char *cmd_str, int cmd_len, int default_stdin,
                              int default_stdout);

#endif // !PARSE_COMMANDS_H
