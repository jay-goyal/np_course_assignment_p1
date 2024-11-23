#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>

// PROJECT IMPORTS
#include "types/cluster_data.h"

#define PIPE_BUF_SIZE 256

extern unsigned int num_process;
extern bool is_cluster;
extern cluster_data_t global_cluster_data;

#endif // !MAIN_H
