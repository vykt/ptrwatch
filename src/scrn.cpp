#include <vector>
#include <string>
#include <optional>
#include <sstream>
#include <variant>

#include <cstdint>
#include <cstring>

#include <curses.h>

#include "scrn.h"
#include "cfg.h"
#include "mem.h"



void scrn::init() {

    //allocate data buffer
    this->data_buf = new byte[128];
    this->next_refollow = true;
    this->terminate = false;

    //init ncurses screen
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    intrflush(stdscr, FALSE);

    //setup colours
    start_color();
    use_default_colors();
    init_pair(1, COLOR_RED, -1);
}


void scrn::fini() {
   
    //delete data buffer
    delete[] this->data_buf;

    //kill ncurses screen
    endwin();
}


//get termination status
bool scrn::get_terminate() {
    return this->terminate;
}


//draw output to screen
void scrn::show_output() {

    bool no_change = true;

    if (this->old_output.size() != 0) {
        //check if output has changed
        for (unsigned long i = 0; i < this->output.size(); ++i) {
            if (this->output[i] != this->old_output[i]) {
                no_change = false;
                break;
            }
        }
    } else {
        no_change = false;
    }

    if (no_change) return;

    //redraw
    clear();
    for (unsigned long i = 0; i < this->output.size(); ++i) {
        if (this->is_title[i]) attron(COLOR_PAIR(1));
        mvprintw(i, 0, this->output[i].c_str()); //non read-only fmt is fine here
        if (this->is_title[i]) attroff(COLOR_PAIR(1));
    }
    refresh();
}


//check if user pressed [space] or [enter] to request a ptr chain refollow
void scrn::check_keyboard() {

    int ch;

    ch = getch();
    if (ch == ' ' || ch == '\n') {
        this->next_refollow = true;
    }
    if (ch == 'q') {
        this->terminate = true;
    }
}


//build output & re-follow each pointer chain
std::optional<std::string> scrn::get_output(cfg * c, mem * m) {

    std::optional<std::string> ret_err;
    std::optional<uintptr_t> ret_off;

    std::string temp_str;

    cfg_entry * temp_c_entry;
    cfg_title * temp_c_title;

    std::vector<cfg_variant> * entries_ptr;
    entries_ptr = c->get_entries();

    //setup vectors
    this->is_title.clear();
    this->old_output = this->output;
    this->output.clear();

    //for each config variant
    for (unsigned long i = 0; i < entries_ptr->size(); ++i) {

        //check if title
        if (std::holds_alternative<cfg_title>((*entries_ptr)[i])) {
           
            //set title colour
            this->is_title.push_back(true);

            //fetch title
            temp_c_title = std::get_if<cfg_title>(&(*entries_ptr)[i]);
            if (temp_c_title == nullptr) {
                return 
                    "[scrn::get_output] variant title fetch returned null.";
            }
            this->output.push_back(temp_c_title->comment);

        //else entry
        } else {

            //set title colour
            this->is_title.push_back(false);

            //clear temp string
            temp_str.clear();

            //fetch entry
            temp_c_entry = std::get_if<cfg_entry>(&(*entries_ptr)[i]);
            if (temp_c_entry == nullptr) {
                return 
                    "[scrn::get_output] variant entry fetch returned null.";
            }

            //re-follow pointer chain if required
            if (this->next_refollow) { 
                ret_off = m->follow_chain(temp_c_entry->start_addr, 
                                          &temp_c_entry->offsets);
                if (ret_off) {
                    temp_c_entry->cached_final_addr = ret_off.value();
                } else {
                    return 
                        "[scrn::get_output] mem::follow_chain() returned null";
                }
            }

            //fetch data
            memset(this->data_buf, 0, DATA_BUF_MAX);
            ret_err = this->build_entry_output(m, temp_c_entry);
            if (ret_err) return ret_err;

        } //end else entry

    } //end for each config variant

    this->next_refollow = false;

    return std::nullopt;
}


//convert data buffer into string according to cfg_entry.c_type
std::optional<std::string> scrn::build_entry_output(mem * m, cfg_entry * temp_c_entry) {

    std::optional<std::string> ret;

    std::ostringstream data_stream;
    std::string temp_string;

    /*  If you know of metaprogramming magic to make the compiler do this
     *  automatically, let me know
     */

    //cast based on type
    switch (temp_c_entry->c_type) {

        case UINT8:
            ret = m->read_addr(temp_c_entry->cached_final_addr, 
                               this->data_buf, sizeof(uint8_t));
            if (ret) return ret;
            data_stream << std::hex << *((uint8_t *) this->data_buf);
            this->output.push_back(data_stream.str());
            break;

        case INT8:
            ret = m->read_addr(temp_c_entry->cached_final_addr, 
                               this->data_buf, sizeof(int8_t));
            if (ret) return ret;
            data_stream << std::hex << *((int8_t *) this->data_buf);
            this->output.push_back(data_stream.str());
            break;

        case UINT16:
            ret = m->read_addr(temp_c_entry->cached_final_addr, 
                               this->data_buf, sizeof(uint16_t));
            if (ret) return ret;
            data_stream << std::hex << *((uint16_t *) this->data_buf);
            this->output.push_back(data_stream.str());
            break;

        case INT16:
            ret = m->read_addr(temp_c_entry->cached_final_addr, 
                               this->data_buf, sizeof(int16_t));
            if (ret) return ret;
            data_stream << std::hex << *((int16_t *) this->data_buf);
            this->output.push_back(data_stream.str());
            break;

        case UINT32:
            ret = m->read_addr(temp_c_entry->cached_final_addr, 
                               this->data_buf, sizeof(uint32_t));
            if (ret) return ret;
            data_stream << std::hex << *((uint32_t *) this->data_buf);
            this->output.push_back(data_stream.str());
            break;

        case INT32:
            ret = m->read_addr(temp_c_entry->cached_final_addr, 
                               this->data_buf, sizeof(int32_t));
            if (ret) return ret;
            data_stream << std::hex << *((int32_t *) this->data_buf);
            this->output.push_back(data_stream.str());
            break;

        case UINT64:
            ret = m->read_addr(temp_c_entry->cached_final_addr, 
                               this->data_buf, sizeof(uint64_t));
            if (ret) return ret;
            data_stream << std::hex << *((uint64_t *) this->data_buf);
            this->output.push_back(data_stream.str());
            break;

        case INT64:
            ret = m->read_addr(temp_c_entry->cached_final_addr, 
                               this->data_buf, sizeof(int64_t));
            if (ret) return ret;
            data_stream << std::hex << *((int64_t *) this->data_buf);
            this->output.push_back(data_stream.str());
            break;

        case FLOAT:
            ret = m->read_addr(temp_c_entry->cached_final_addr, 
                               this->data_buf, sizeof(float));
            if (ret) return ret;
            data_stream << std::dec << *((float *) this->data_buf);
            this->output.push_back(data_stream.str());
            break;

        case DOUBLE:
            ret = m->read_addr(temp_c_entry->cached_final_addr, 
                               this->data_buf, sizeof(double));
            if (ret) return ret;
            data_stream << std::dec << *((double *) this->data_buf);
            this->output.push_back(data_stream.str());
            break;

        case CHAR:
            ret = m->read_addr(temp_c_entry->cached_final_addr, 
                               this->data_buf, sizeof(char));
            if (ret) return ret;
            data_stream << *((char *) this->data_buf);
            this->output.push_back(data_stream.str());
            break;

        case STRING:
            ret = m->read_addr(temp_c_entry->cached_final_addr, 
                               this->data_buf, temp_c_entry->str_len);
            if (ret) return ret;
            data_stream.write((const char *) this->data_buf, temp_c_entry->str_len);
            data_stream << '\n';
            this->output.push_back(data_stream.str());
            break;

    } //end switch

    return std::nullopt;
}
