#include <vector>
#include <string>
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

    //init ncurses screen
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    intrflush(stdscr, FALSE);
}


void scrn::end() {
   
    //delete data buffer
    delete[] this->data_buf;

    //kill ncurses screen
    endwin();
}


//draw output to screen
void scrn::show_output() {

    clear();
    for (int i = 0; i < this->output.size(); ++i) {
        mvprintw(i, 0, this->output[i].c_str()); //non read-only fmt is fine here
    }
    refresh();
}


//check if user pressed [space] or [enter] to request a ptr chain refollow
void scrn::check_refollow() {

    int ch;

    ch = getch();
    if (ch == ' ' || ch == '\n') {
        this->next_refollow = true;
    }
}


//build output & re-follow each pointer chain
void scrn::get_output(cfg * c, mem * m) {

    int ret;

    std::string temp_str;

    cfg_entry * temp_c_entry;
    cfg_title * temp_c_title;

    std::vector<cfg_variant> * entries_ptr;
    entries_ptr = c->get_entries();

    //empty out output vector
    this->output.clear();

    //for each config variant
    for (int i = 0; i < entries_ptr->size(); ++i) {

        //check if title
        if (std::holds_alternative<cfg_title>((*entries_ptr)[i])) {
            
            //fetch title
            temp_c_title = std::get_if<cfg_title>(&(*entries_ptr)[i]);
            if (temp_c_title == nullptr) {
                //TODO handle error
            }
            this->output.push_back(temp_c_title->comment);

        //else entry
        } else {

            //clear temp string
            temp_str.clear();

            //fetch entry
            temp_c_entry = std::get_if<cfg_entry>(&(*entries_ptr)[i]);
            if (temp_c_entry == nullptr) {
                //TODO handle error
            }

            //re-follow pointer chain if required
            if (this->next_refollow) { 
                temp_c_entry->cached_final_addr = 
                    m->follow_chain(temp_c_entry->start_addr,
                                    &temp_c_entry->offsets);
                this->next_refollow = false;
            }

            //fetch data
            memset(this->data_buf, 0, DATA_BUF_MAX);
            //TODO WRITE A TEMPLATE FUNCTION FOR READING DATA TYPE & CONVERT TO STR

        } //end else entry

    } //end for each config variant

}


//convert data buffer into string according to cfg_entry.c_type
void scrn::build_entry_output(mem * m, cfg_entry * temp_c_entry) {

    int ret;

    std::ostringstream data_stream;
    std::string temp_string;

    /*  If you know of metaprogramming magic to make the compiler do this
     *  automatically, let me know
     */

    //cast based on type
    switch (temp_c_entry->c_type) {

        //TODO handle read_addr fail

        case UINT8:
            ret = m->read_addr(temp_c_entry->cached_final_addr, 
                               this->data_buf, sizeof(uint8_t));
            data_stream << *((uint8_t *) this->data_buf);
            this->output.push_back(data_stream.str());
            break;

        case INT8:
            ret = m->read_addr(temp_c_entry->cached_final_addr, 
                               this->data_buf, sizeof(int8_t));
            data_stream << *((int8_t *) this->data_buf);
            this->output.push_back(data_stream.str());
            break;

        case UINT16:
            ret = m->read_addr(temp_c_entry->cached_final_addr, 
                               this->data_buf, sizeof(uint16_t));
            data_stream << *((uint16_t *) this->data_buf);
            this->output.push_back(data_stream.str());
            break;

        case INT16:
            ret = m->read_addr(temp_c_entry->cached_final_addr, 
                               this->data_buf, sizeof(int16_t));
            data_stream << *((int16_t *) this->data_buf);
            this->output.push_back(data_stream.str());
            break;

        case UINT32:
            ret = m->read_addr(temp_c_entry->cached_final_addr, 
                               this->data_buf, sizeof(uint32_t));
            data_stream << *((uint32_t *) this->data_buf);
            this->output.push_back(data_stream.str());
            break;

        case INT32:
            ret = m->read_addr(temp_c_entry->cached_final_addr, 
                               this->data_buf, sizeof(int32_t));
            data_stream << *((int32_t *) this->data_buf);
            this->output.push_back(data_stream.str());
            break;

        case UINT64:
            ret = m->read_addr(temp_c_entry->cached_final_addr, 
                               this->data_buf, sizeof(uint64_t));
            data_stream << *((uint64_t *) this->data_buf);
            this->output.push_back(data_stream.str());
            break;

        case INT64:
            ret = m->read_addr(temp_c_entry->cached_final_addr, 
                               this->data_buf, sizeof(int64_t));
            data_stream << *((int64_t *) this->data_buf);
            this->output.push_back(data_stream.str());
            break;

        case FLOAT:
            ret = m->read_addr(temp_c_entry->cached_final_addr, 
                               this->data_buf, sizeof(float));
            data_stream << *((float *) this->data_buf);
            this->output.push_back(data_stream.str());
            break;

        case DOUBLE:
            ret = m->read_addr(temp_c_entry->cached_final_addr, 
                               this->data_buf, sizeof(double));
            data_stream << *((double *) this->data_buf);
            this->output.push_back(data_stream.str());
            break;

        case CHAR:
            ret = m->read_addr(temp_c_entry->cached_final_addr, 
                               this->data_buf, sizeof(char));
            data_stream << *((char *) this->data_buf);
            this->output.push_back(data_stream.str());
            break;

        case STRING:
            ret = m->read_addr(temp_c_entry->cached_final_addr, 
                               this->data_buf, temp_c_entry->str_len);
            data_stream.write((const char *) this->data_buf, temp_c_entry->str_len);
            data_stream << '\n';
            this->output.push_back(data_stream.str());
            break;

    } //end switch

}
