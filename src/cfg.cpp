#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include <cstdint>

#include <libpwu.h>

#include "cfg.h"
#include "mem.h"


//read config
int cfg::read_cfg(mem * m) {

    int ret;

    std::vector<std::string> substrings;
    std::string line;

    int substring_count;

    cfg_entry temp_c_entry;
    cfg_title temp_c_title;


    //process cfg a line at a time
    while (std::getline(*this->stream, line)) {

        substring_count = 0;
        substrings.clear();

        //check if this line is a title
        ret = check_title(&line);
        if (ret == 0) {
            
            temp_c_title.comment = line;
            this->entries.push_back(temp_c_title);

            continue;
        }

        //otherwise this is an entry
        
        //divide line into substrings
        ret = line_to_substrings(&line, &substrings, &substring_count);
        if (ret == -1) continue;

        //check substring formatting
        ret = check_type_format(&substrings);
        if (ret == -1) {
            //TODO report error
        }
        temp_c_entry.c_type = (enum cfg_type) ret;

        //save length if type is string to cfg_type
        if (temp_c_entry.c_type == STRING) {
            temp_c_entry.str_len = std::stoi(substrings[1]);
            substrings.erase(substrings.begin() + 1);
        }

        //divide offset substring into offsets
        ret = substring_to_offsets(m, &substrings[1], 
                                   &temp_c_entry.start_addr, &temp_c_entry.offsets);
        if (ret == -1) {
            //TODO report error
        }

        //add temp_c_entry to config vector
        this->entries.push_back(temp_c_entry);

    } //end while each line

    return 0;
}


//get entries for building output
std::vector<cfg_variant> * cfg::get_entries() {
    
    return &(this->entries);
}


//return 0 if this line is a title
int cfg::check_title(std::string * line) {

    if ((*line)[0] == '>') return 0;
    return -1;
}


//split line into substrings deliminated by ':'
inline int cfg::line_to_substrings(std::string * line,
                                   std::vector<std::string> * substrings,
                                   int * substring_count) {

    std::string substring;

    //skip comments, blank lines and improperly formatted lines
    if ((*line)[0] == '#' || (*line)[0] == '\n' || (*line)[0] == ' ') return -1;

    //convert line to stream
    std::istringstream line_stream(*line);

    //get substrings
    while (std::getline(line_stream, substring, ':')) {
        substrings->push_back(substring);
        *substring_count += 1;
    }

    return 0;
}


//get the first address in the final substring
inline uintptr_t cfg::get_first_addr(mem * m, std::string * substring) {

    int ret;
    uintptr_t first_addr;
    maps_obj * m_obj;
    maps_entry * first_entry;

    //check if using a raw address
    if ((*substring)[0] == '0' && (*substring)[1] == 'x') {
        first_addr = std::stoull(*substring, nullptr, 16);
        return first_addr;
    }

    //otherwise its a backing file

    //get backing object segment group
    ret = get_obj_by_basename((char *) substring->c_str(), m->get_m_data(), &m_obj);
    if (ret != 0) {
        //TODO report error
    }

    //get first segment of group
    ret = vector_get(&m_obj->entry_vector, 0, (byte *) &first_entry);
    if (ret == -1) {
        //TODO report error
    }

    first_addr = (uintptr_t) first_entry->start_addr;
    return first_addr;
}


//split substring into offsets deliminated by ' '
inline int cfg::substring_to_offsets(mem * m, std::string * substring, 
                                     uintptr_t * start_addr, 
                                     std::vector<uintptr_t> * offsets) {

    std::string offset;
    int offset_count = 0;

    //convert last substring to stream
    std::istringstream offset_stream(*substring);
    
    //get offsets
    while (std::getline(offset_stream, offset, ' ')) {
        
        //if first iteration, need to resolve name 
        if (offset_count == 0) {
            *start_addr = get_first_addr(m, &offset);
        } else {
            offsets->push_back(std::stoull(offset, nullptr, 16));
        }
        ++offset_count;
    }

    //check at least a starting address was provided
    if (offset_count == 0) {
        //TODO report error
    }

    return 0;
}


//convert type substring into type enum
inline int cfg::check_type_format(std::vector<std::string> * substrings) {

    //types
    const std::string cfg_type[CFG_TYPES_LEN] = {
    "uint8",
    "int8",
    "uint16",
    "int16",
    "uint32",
    "int32",
    "uint64",
    "int64",
    "float",
    "double",
    "char",
    "string"
    };

    int substring_type = -1;

    //check substrings are properly formatted
    for (int i = 0; i < CFG_TYPES_LEN; ++i) {
        if ((*substrings)[i] == cfg_type[i]) substring_type = i;
    }
    if (substring_type == -1) {
        return -1;
    }

    //check length
    if (!((substrings->size() == 2 && substring_type != 11)
        || (substrings->size() == 3 && substring_type == 11))) {
        return -1;
    }

    return substring_type;

}
