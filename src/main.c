#include <stdlib.h>
#include <unistd.h>

// PROJECT IMPORTS
#include "signal_handlers.h"
#include "utils/printfn.h"

int main(int argc, char *argv[])
{
    setup_sighandlers();
    pid_t pid = fork();
    if(pid == 0)
        PRINTF_FG_CYAN("PRINTF FROM CHILD\n");
    else
        while(1)
            ;

    return EXIT_SUCCESS;
}
