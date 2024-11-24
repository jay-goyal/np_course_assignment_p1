#ifndef MISC_H
#define MISC_H

#include <arpa/inet.h>

void exit_err_status(char *fmt);
char *in_addr_to_str(in_addr_t addr);

#endif  // !MISC_H
