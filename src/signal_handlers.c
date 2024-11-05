#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>

// PROJECT IMPORTS
#include "main.h"
#include "signal_handlers.h"
#include "utils/printfn.h"

void sigchld_handler(int signo)
{
    if(signo != SIGCHLD)
    {
        PRINTF_FG_RED("Invalid signal received");
        PRINTF_FG_WHITE("\n");
    }
    pid_t pid;
    int status;
    int flags = WNOHANG;
    while(1)
    {
        pid = waitpid(-1, &status, WNOHANG);
        if(pid == 0 || pid == -1)
            return;
        num_process -= 1;
        PRINTF_FG_GREEN("Process with PID %d exited with exit code %d", pid,
                        WEXITSTATUS(status));
        PRINTF_FG_WHITE("\n");
    }
}

void setup_sighandlers(void)
{
    PRINTF_FG_CYAN("INITIALIZING SIGNAL HANDLERS");
    PRINTF_FG_WHITE("\n");
    signal(SIGCLD, sigchld_handler);
}
