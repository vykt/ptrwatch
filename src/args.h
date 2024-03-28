#ifndef ARGS_H
#define ARGS_H

#include <string>

#include "unistd.h"


typedef struct {

    pid_t target_pid;
    std::string cfg_str;

} args_struct;


int process_args(int argc, char ** argv, args_struct * args);


#endif
