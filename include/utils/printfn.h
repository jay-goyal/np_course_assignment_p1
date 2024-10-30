#ifndef PRINTFN_H
#define PRINTFN_H

#include <stdio.h>

#define FG_RST "\x1b[0m"
#define FG_RED "\x1b[31m"
#define FG_GREEN "\x1b[32m"
#define FG_YELLOW "\x1b[33m"
#define FG_BLUE "\x1b[34m"
#define FG_MAGENTA "\x1b[35m"
#define FG_CYAN "\x1b[36m"

#define PRINTF_FG_WHITE(fmt, ...) printf(FG_RST fmt, ##__VA_ARGS__)
#define PRINTF_FG_RED(fmt, ...) printf(FG_RED fmt FG_RST, ##__VA_ARGS__)
#define PRINTF_FG_GREEN(fmt, ...) printf(FG_GREEN fmt FG_RST, ##__VA_ARGS__)
#define PRINTF_FG_YELLOW(fmt, ...) printf(FG_YELLOW fmt FG_RST, ##__VA_ARGS__)
#define PRINTF_FG_BLUE(fmt, ...) printf(FG_BLUE fmt FG_RST, ##__VA_ARGS__)
#define PRINTF_FG_MAGENTA(fmt, ...)                                           \
    printf(FG_MAGENTA fmt FG_RST, ##__VA_ARGS__)
#define PRINTF_FG_CYAN(fmt, ...) printf(FG_CYAN fmt FG_RST, ##__VA_ARGS__)

#endif // !PRINTFN_H
