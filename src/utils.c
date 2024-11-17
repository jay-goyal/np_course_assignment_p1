#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// PROJECT IMPORTS
#include "utils/misc.h"

void exit_err_status(char *fmt)
{
    perror(fmt);
    exit(-1);
}
