#ifndef ARGS_H
#define ARGS_H

#include <string>
#include <optional>

#include "unistd.h"


typedef struct {

    pid_t target_pid;
    std::string cfg_str;

} args_struct;


std::optional<std::string> process_args(int argc, char ** argv, args_struct * args);


#endif
