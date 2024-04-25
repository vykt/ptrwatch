#include <vector>
#include <string>
#include <optional>

#include <cstdio>
#include <cstdint>

#include <unistd.h>

#include <libpwu.h>

#include "mem.h"
#include "args.h"



//init class
std::optional<std::string> mem::init(args_struct * args) {

   int ret;

    //allocate maps_data object
    ret = new_maps_data(&this->m_data);
    if (ret) {
        return 
            "[mem::init] libpwu->new_maps_data() could not allocate memory for maps.";
    }

    //open target process procfs memory and maps
    ret = open_memory(args->target_pid, &this->stream_maps, &this->fd_mem);
    if (ret) {
        return 
            "[mem::init] libpwu->open_memory() failed to open target maps & memory.";
    }

    //read target process memory maps into maps_data
    ret = read_maps(&this->m_data, this->stream_maps);
    if (ret) {
        return
            "[mem::init] libpwu->read_maps() error while reading target procfs maps.";
    }

    return std::nullopt;
}

std::optional<std::string> mem::fini() {

    int ret;

    //deallocate maps_data structure
    ret = del_maps_data(&this->m_data);
    if (ret) {
        return
            "[mem::fini] libpwu->del_maps_data() error deallocating.";
    }

    //close mem descriptor & maps stream
    ret = close(this->fd_mem);
    if (ret) {
        return
            "[mem::fini] error closing open procfs memory descriptor.";
    }

    ret = fclose(this->stream_maps);
    if (ret) {
        return
            "[mem::fini] error closing open procfs maps stream.";
    }

    return std::nullopt;
}



//libpwu main structure getter
maps_data * mem::get_m_data() {
    return &this->m_data;
}


//fd for /proc/<pid>/mem
int mem::get_fd_mem() {
    return this->fd_mem;
}


//read data at addr
std::optional<std::string> mem::read_addr(uintptr_t addr, byte * buf, size_t size) {

    off_t ret;
    ssize_t read_bytes;

    ret = lseek(this->fd_mem, addr, SEEK_SET);
    if (ret == -1) {
        return "[mem::read_addr] lseek syscall returned -1.";
    }

    read_bytes = read(this->fd_mem, buf, size);
    if (read_bytes == -1) {
        return "[mem::read_addr] read syscall returned -1.";
    }

    return std::nullopt;
}


//dereference chain 
std::optional<uintptr_t> mem::follow_chain(uintptr_t start_addr, 
                       std::vector<uintptr_t> * offsets) {

    off_t ret;
    ssize_t read_bytes;
    uintptr_t final_addr;

    //for each offset
    for (unsigned long i = 0; i < offsets->size() - 1; ++i) {

        ret = lseek(this->fd_mem, start_addr + (*offsets)[i], SEEK_SET);
        if (ret == -1) {
            return std::nullopt;
        }

        read_bytes = read(this->fd_mem, &start_addr, sizeof(start_addr));
        if (read_bytes == -1) {
            return std::nullopt;
        }

    } //end for each offset
   
    //get address of value at the end of the pointer chain
    final_addr = start_addr + (*offsets)[offsets->size() - 1];

    return final_addr;
}
