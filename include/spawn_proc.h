#ifndef SPAWN_PROC_H
#define SPAWN_PROC_H

#include "types/cmd.h"
void spawn_proc(command_t *cmd, int *pipefds, size_t num_pipes);

#endif // !SPAWN_PROC_H
