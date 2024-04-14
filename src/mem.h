#ifndef MEM_H
#define MEM_H

#include <vector>
#include <string>
#include <optional>

#include <cstdio>
#include <cstdint>

#include <unistd.h>

#include <libpwu.h>

#include "args.h"


class mem {

    //attributes
    private:
    maps_data m_data;
    int fd_mem;
    FILE * stream_maps;

    //methods:
    public:
    std::optional<std::string> init(args_struct * args);
    std::optional<std::string> fini();

    maps_data * get_m_data();
    int get_fd_mem();

    std::optional<std::string> read_addr(uintptr_t addr, byte * buf, size_t size);
    std::optional<uintptr_t> follow_chain(uintptr_t start_addr, 
                                          std::vector<uintptr_t> * offsets);
};

#endif
