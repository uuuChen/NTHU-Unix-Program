# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <dlfcn.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <dirent.h>
# include <vector>
# include <stdbool.h>

# include <iostream>
# include <string>
# include <regex>

# define DEFAULT_SOPATH "./sandbox.so"
# define DEFAULT_BASEDIR "."
# define MAX_CMD_BUFFER 128
# define MAX_CMDS_NUM_PER_INPUT 16

using namespace std;

void print_usage(void){
    printf("usage: ./sandbox [-p sopath] [-d basedir] [--] cmd [cmd args ...]\n \
    \t -p: set the path to sandbox.so, default = ./sandox.so\n \
    \t -d: restrict directory, default = .\n \
    \t --: seperate the arguments for sandbox and for the executed command\n\n");
}

bool is_cmd_valid(string cmd){
    regex reg("exec*"); 
    return ! (regex_match(cmd, reg) || cmd == "system");
}

int main(int argc, char **argv)
{
    int sandbox_cmd = 0;

    string sopath = DEFAULT_SOPATH;
    string basedir = DEFAULT_BASEDIR;

    while ((sandbox_cmd = getopt(argc, argv, "p:d:")) != -1){

        switch(sandbox_cmd){
	    case 'p':
	        sopath = optarg;
		break;

	    case 'd':
	        basedir = optarg;
		break;

	    case '?':
	        print_usage();
		exit(EXIT_FAILURE);
	}
    }

    if (argc > optind){
	string system_cmd = "";
        string user_cmd = "";
	string user_cmd_args = "";
	string redirect_file_path = "";

	user_cmd = string(argv[optind++]);
	while(optind < argc){
	    user_cmd_args += " " + string(argv[optind++]);
	}

        if (is_cmd_valid(user_cmd)){
	    system_cmd = "valid" + user_cmd + user_cmd_args;
	}else{
	    system_cmd = user_cmd + user_cmd_args;
	}
	system(system_cmd.c_str());
    	
    }
    return 0;
}
