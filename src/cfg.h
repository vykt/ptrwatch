#ifndef CFG_H
#define CFG_H

#include <vector>
#include <string>
#include <optional>
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
    std::ifstream stream;
    std::vector<cfg_variant> entries;

    //methods
    public:
    std::optional<std::string> open_cfg(std::string path);
    void close_cfg();
    
    std::optional<std::string> read_cfg(mem * m);

    std::vector<cfg_variant> * get_entries();

    private:
    std::optional<int> check_title(std::string * line);
    std::optional<int> line_to_substrings(std::string * line, 
                                          std::vector<std::string> * substrings);
    std::optional<uintptr_t> get_first_addr(mem * m, std::string * substring);
    std::optional<std::string> substring_to_offsets(mem * m, std::string * substring,
                                                    uintptr_t * start_addr, 
                                                    std::vector<uintptr_t> * offsets);
    std::optional<std::string> check_multiply_offset(std::string * offset_str,
                                                     uintptr_t * offset);
    std::optional<int> check_type_format(std::vector<std::string> * substrings);

};

#endif
