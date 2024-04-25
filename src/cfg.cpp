#include <vector>
#include <string>
#include <optional>
#include <fstream>
#include <iostream>
#include <sstream>

#include <cstdint>

#include <libpwu.h>

#include "cfg.h"
#include "scrn.h"
#include "mem.h"


//open stream on config file
std::optional<std::string> cfg::open_cfg(std::string path) {

    this->stream.open(path.c_str());
    if (!this->stream) return "[cfg::open_cfg] failed to open specified config file.";

    return std::nullopt;
}


//close stream on config file
void cfg::close_cfg() {

    this->stream.close();
}


//read config
std::optional<std::string> cfg::read_cfg(mem * m) {

    std::optional<std::string> ret_err;
    std::optional<int> ret_int;
    std::optional<uintptr_t> ret_off;

    std::vector<std::string> substrings;
    std::string line;

    cfg_entry temp_c_entry;
    cfg_title temp_c_title;


    //process cfg a line at a time
    while (std::getline(this->stream, line)) {

        temp_c_entry.offsets.clear();
        temp_c_title.comment.clear();
        substrings.clear();

        //check if this line is a title
        ret_int = check_title(&line);
        if (ret_int) {
            line.erase(0, 1);
            temp_c_title.comment = line;
            this->entries.push_back(temp_c_title);

            continue;
        }

        //otherwise this is an entry
        
        //divide line into substrings
        ret_int = line_to_substrings(&line, &substrings);
        if (!ret_int) continue;

        //check substring formatting
        ret_int = check_type_format(&substrings);
        if (!ret_int) {
            return "[cfg::read_cfg] type format is improperly formatted.";
        }
        temp_c_entry.c_type = (enum cfg_type) ret_int.value();

        //save length if type is string to cfg_type
        if (temp_c_entry.c_type == STRING) {
            temp_c_entry.str_len = std::stoi(substrings[1]);
            substrings.erase(substrings.begin() + 1);
        }

        //divide offset substring into offsets
        ret_err = substring_to_offsets(m, &substrings[1], 
                                   &temp_c_entry.start_addr, &temp_c_entry.offsets);
        if (ret_err) {
            return ret_err;
        }

        //add temp_c_entry to config vector
        this->entries.push_back(temp_c_entry);

    } //end while each line

    return std::nullopt;
}


//get entries for building output
std::vector<cfg_variant> * cfg::get_entries() {
    
    return &(this->entries);
}


//return 0 if this line is a title
std::optional<int> cfg::check_title(std::string * line) {

    if ((*line)[0] == '>') return 0;
    return std::nullopt;
}


//split line into substrings deliminated by ':'
inline std::optional<int> cfg::line_to_substrings(std::string * line,
                                            std::vector<std::string> * substrings) {

    int substring_count;
    std::string substring;

    substring_count = 0;

    //skip comments, blank lines and improperly formatted lines
    if (!line->length() 
        || (*line)[0] == '#' || (*line)[0] == '\n' || (*line)[0] == ' ') {
        return std::nullopt;
    }

    //convert line to stream
    std::istringstream line_stream(*line);

    //get substrings
    while (std::getline(line_stream, substring, ':')) {
        substrings->push_back(substring);
        substring_count += 1;
    }

    return substring_count;
}


//get the first address in the final substring
inline std::optional<uintptr_t> cfg::get_first_addr(mem * m, std::string * substring) {

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
        return std::nullopt;
    }

    //get first segment of group
    ret = vector_get(&m_obj->entry_vector, 0, (byte *) &first_entry);
    if (ret == -1) {
        return std::nullopt;
    }

    first_addr = (uintptr_t) first_entry->start_addr;
    return first_addr;
}


//split substring into offsets deliminated by ' '
inline std::optional<std::string> cfg::substring_to_offsets(mem * m, 
                                                    std::string * substring, 
                                                    uintptr_t * start_addr, 
                                                    std::vector<uintptr_t> * offsets) {
    int offset_count;
    uintptr_t offset_ptr;

    std::string offset;
    std::optional<std::string> ret_err;
    std::optional<uintptr_t> ret_off;

    offset_count = 0;

    //convert last substring to stream
    std::istringstream offset_stream(*substring);
    
    //get offsets
    while (std::getline(offset_stream, offset, ' ')) {
        
        //if first iteration, need to resolve name 
        if (offset_count == 0) {
            ret_off = get_first_addr(m, &offset);
            if (!ret_off) return
                "[cfg::substring_to_offsets] could not resolve first address.";
            else {
                *start_addr = ret_off.value();
            }
        } else {
            ret_err = this->check_multiply_offset(&offset, &offset_ptr);
            if (ret_err) {
                return ret_err;
            }

            offsets->push_back(offset_ptr);
        }
        ++offset_count;
    }

    //check at least a starting address was provided
    if (offset_count == 0) {
        return 
          "[cfg::substring_to_offsets] no starting address or offsets were provided.";
    }

    return std::nullopt;
}


//multiply offset string if necessary (e.g;
inline std::optional<std::string> cfg::check_multiply_offset(std::string * offset_str,
                                                             uintptr_t * offset) {
    uintptr_t offset_ptr;

    std::string offset_value;
    std::string offset_mult;

    //convert offset string to stream
    std::stringstream offset_stream(*offset_str);

    //get offset
    std::getline(offset_stream, offset_value, '*');
    offset_ptr = std::stoull(offset_value, nullptr, 16);

    //get multiplier if applicable
    if(!std::getline(offset_stream, offset_mult, '*')) {
        //no multiplier
        *offset = offset_ptr;
        return std::nullopt;
    } else {
        //multiplier present
        offset_ptr *= std::stoull(offset_mult, nullptr, 16);
        *offset = offset_ptr;
        return std::nullopt; 
    }
}


//convert type substring into type enum
inline std::optional<int> 
    cfg::check_type_format(std::vector<std::string> * substrings) {

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
        if ((*substrings)[0] == cfg_type[i]) substring_type = i;
    }
    if (substring_type == -1) {
        return std::nullopt;
    }

    //check length
    if (!((substrings->size() == 2 && substring_type != 11)
        || (substrings->size() == 3 && substring_type == 11))) {
        return std::nullopt;
    }

    return substring_type;

}
