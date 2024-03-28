#ifndef SCRN_H
#define SCRN_H

#include <vector>
#include <string>

#include <curses.h>

#include "cfg.h"
#include "mem.h"


#define DATA_BUF_MAX 128


//ncurses & output class
class scrn {

    //attributes
    private:
    byte * data_buf;
    std::vector<std::string> output;

    bool next_refollow;

    //methods
    public:
    void init();
    void end();

    void show_output();
    void check_refollow();

    private:
    void get_output(cfg * c, mem * m);
    void build_entry_output(mem * m, cfg_entry * temp_c_entry);
};

#endif
