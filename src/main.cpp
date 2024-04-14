#include <iostream>
#include <optional>

#include "cfg.h"
#include "mem.h"
#include "args.h"
#include "scrn.h"


#define RESET "\033[0m"
#define RED   "\033[31m"


void report_error(std::optional<std::string> error) {

    std::string error_str = *error;
    std::cerr << RED << "[ERROR] " << RESET << error_str << std::endl;
}


int main(int argc, char ** argv) {

    std::optional<std::string> ret, ret_saved;

    args_struct args;
    cfg c;
    mem m;
    scrn s;

    //get arguments
    ret = process_args(argc, argv, &args);
    if (ret) report_error(ret);


    //init libpwu
    ret = m.init(&args);
    if (ret) report_error(ret);


    //read config file
    ret = c.open_cfg(args.cfg_str);
    if (ret) report_error(ret);

    ret = c.read_cfg(&m);
    if (ret) report_error(ret);


    //init ncurses
    s.init();

    //display loop
    while (!s.get_terminate()) {

        ret_saved = s.get_output(&c, &m);
        if (ret_saved) {
            break;            
        }
        s.show_output();
        s.check_keyboard();

    } //end display loop


    //clean up
    s.fini();

    if (ret_saved) report_error(ret_saved);

    ret = m.fini();
    if (ret) report_error(ret);
    
    c.close_cfg();
}
