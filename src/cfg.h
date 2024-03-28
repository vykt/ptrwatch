#ifndef CFG_H
#define CFG_H

#include <vector>
#include <string>
#include <variant>
#include <fstream>
#include <iostream>

#include <cstdint>

#include "mem.h"


#define CFG_TYPES_LEN 12


//data types allowed inside config
enum cfg_type {

    UINT8 = 0,
    INT8 = 1,
    UINT16 = 2,
    INT16 = 3,
    UINT32 = 4,
    INT32 = 5,
    UINT64 = 6,
    INT64 = 7,
    FLOAT = 8,
    DOUBLE = 9,
    CHAR = 10,
    STRING = 11
};


//entry inside config
typedef struct {

    enum cfg_type c_type;
    int str_len;
    uintptr_t start_addr;
    std::vector<uintptr_t> offsets;
    uintptr_t cached_final_addr;

} cfg_entry;

//title inside config
typedef struct {
    
    std::string comment;

} cfg_title;


//each line in config is either an entry or a comment
using cfg_variant = std::variant<cfg_entry, cfg_title>;


class cfg {

    //attributes
    private:
    std::ifstream * stream;
    std::vector<cfg_variant> entries;

    //methods
    public:
    int open_cfg(std::string path);
    int close_cfg(std::string path);
    int read_cfg(mem * m);

    std::vector<cfg_variant> * get_entries();

    private:
    int check_title(std::string * line);
    int line_to_substrings(std::string * line,
                           std::vector<std::string> * substrings, 
                           int * substring_count); 
    uintptr_t get_first_addr(mem * m, std::string * substring);
    int substring_to_offsets(mem * m, std::string * substring,
                             uintptr_t * start_addr, std::vector<uintptr_t> * offsets);
    int check_type_format(std::vector<std::string> * substrings);

};

#endif
