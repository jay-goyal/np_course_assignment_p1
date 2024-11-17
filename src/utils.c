#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// PROJECT IMPORTS
#include "utils/misc.h"
#include "utils/printfn.h"

void exit_err_status(char *fmt)
{
    PRINTF_FG_RED("%s", fmt);
    PRINTF_FG_WHITE("\n");
    exit(-1);
}
