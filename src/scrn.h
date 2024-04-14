#ifndef SCRN_H
#define SCRN_H

#include <vector>
#include <string>
#include <optional>

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
    bool terminate;

    //methods
    public:
    void init();
    void fini();

    bool get_terminate();
    void show_output();
    void check_keyboard();
    std::optional<std::string> get_output(cfg * c, mem * m);

    private:
    std::optional<std::string> build_entry_output(mem * m, cfg_entry * temp_c_entry);
};

#endif
