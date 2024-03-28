#include <string>

#include <unistd.h>
#include <getopt.h>

#include <linux/limits.h>

#include <libpwu.h>

#include "args.h"
#include "cfg.h"


//convert target into PID
int interpret_target(args_struct * args, char ** argv) {

    int ret;
    char proc_name[NAME_MAX] = {};

    std::string target_str = argv[optind];


	//if the target string contains only digits then interpret as PID
	if (target_str.find_first_not_of("0123456789") == std::string::npos) {
        
        //set pid
        args->target_pid = (pid_t) std::stoi(target_str);
        
        //get process name
        ret = name_by_pid(args->target_pid, proc_name);
        if (ret) {
            //TODO report error (process doesn't exist?)
        }
        return 0;
    }

    //otherwise interpret as process name

    //initialise name_pid structure
    name_pid n_pid;
    ret = new_name_pid(&n_pid, (char *) argv[optind]);
    if (ret) {
        //TODO report error
    }

    //fetch PIDs for name & deal with result
    ret = pid_by_name(&n_pid, &args->target_pid);
    switch (ret) {
        case -1:
            //TODO return error (failed to perform search)
            break;
        case 0:
            //TODO report error (no process found with supplied name)
            break;
        case 1:
            break;
        default:
            //TODO report error (multiple matches found, please provide PID instead)
            break;
    }

    ret = del_name_pid(&n_pid);
    //not worth exception on fail

    return 0;

}


//process argv
int process_args(int argc, char ** argv, args_struct * args) {

    int ret, opt, opt_index;

    struct option long_opts[] = {
        {"config", required_argument, NULL, 'c'},
        {0,0,0,0}
    };

    //set defaults
    args->cfg_str = "./watchchain.cfg";
    
    //option processing while loop
    while((opt = getopt_long(argc, argv, "c:", 
           long_opts, &opt_index)) != -1 
          && opt != 0) {

        //determine parsed argument
        switch (opt) {

            case 'c': //specify config path
                args->cfg_str = optarg;
                break;

        } //end switch
    } //end opt while loop

    //get target_pid
    if (argv[optind] == 0) {
        //TODO report error
    }

    ret = interpret_target(args, argv);
    if (ret) {
        //TODO report error
    }

    return 0;
}
