#ifndef SIGNAL_HANDLERS_H
#define SIGNAL_HANDLERS_H

void sigchld_handler(int signo);
void setup_sighandlers(void);

#endif // !SIGNAL_HANDLERS_H
