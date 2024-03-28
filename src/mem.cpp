#include <vector>

#include <cstdio>
#include <cstdint>

#include <unistd.h>

#include <libpwu.h>

#include "mem.h"
#include "args.h"


//TODO write init method to open m_data, read_maps, and open fd_mem
//     write fini method to close m_data, close fd_mem

//init class
int mem::init(args_struct * args) {

   int ret;

    //allocate maps_data object
    ret = new_maps_data(&this->m_data);
    if (ret) {
        //TODO report error
    }

    //open target process procfs memory and maps
    ret = open_memory(args->target_pid, &this->stream_maps, &this->fd_mem);
    if (ret) {
        //TODO report error (failed to open target process memory)
    }

    //read target process memory maps into maps_data
    ret = read_maps(&this->m_data, this->stream_maps);
    if (ret) {
        //TODO report error (failed to populate maps_data structure
    }

    return 0;
}

int mem::fini() {

    int ret;

    //deallocate maps_data structure
    ret = del_maps_data(&this->m_data);
    if (ret) {
        //TODO report error (failed to free memory)
    }

    //close mem descriptor & maps stream
    ret = close(this->fd_mem);
    if (ret) {
        //TODO try flushing kernel buffer?
    }

    ret = fclose(this->stream_maps);
    if (ret) {
        //TODO try flushing libc buffer?
    }

    return 0;
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
int mem::read_addr(uintptr_t addr, byte * buf, size_t size) {

    ssize_t read_bytes;

    read_bytes = read(this->fd_mem, buf, size);
    if (read_bytes == -1) {
        //TODO report error
    }

    return 0;
}


//dereference chain 
uintptr_t mem::follow_chain(uintptr_t start_addr, 
                       std::vector<uintptr_t> * offsets) {

    off_t ret;
    ssize_t read_bytes;
    uintptr_t final_addr;

    //for each offset
    for (int i = 0; i < offsets->size() - 1; ++i) {

        ret = lseek(this->fd_mem, start_addr + (*offsets)[i], SEEK_SET);
        if (ret == -1) {
            //TODO report error
        }

        read_bytes = read(this->fd_mem, &start_addr, sizeof(start_addr));
        if (read_bytes == -1) {
            //TODO report error
        }

    } //end for each offset
   
    //get address of value at the end of the pointer chain
    final_addr = start_addr + (*offsets)[offsets->size() - 1];

    return final_addr;
}
