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
# define DEFAULT_BASEDIR "./"
# define MAX_CMD_BUF 128
# define MAX_CMDS_NUM_PER_INPUT 16

extern char** environ;

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

bool is_cmd_exist_in_shell(string cmd, string& cmd_dir){
    FILE *fp;
    string cmd_dirs[2] = {"/bin/", "/usr/bin/"};
    int i;
    for(i=0; i<2; i++){
	string cmd_path = cmd_dirs[i] + cmd;
        if((fp=fopen(cmd_path.c_str(), "r")) != NULL){
	    cmd_dir = cmd_dirs[i];
	    return true;
	}
    }
    return false;
}

void print_envp(void){
    char **envir = environ;
    while(*envir){
        fprintf(stdout, "%s\n", *envir);
	envir++;
    }
    fprintf(stdout, "\n\n\n");
}

int main(int argc, char *argv[], char** envp)
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

    // print_envp();
    setenv("LD_PRELOAD", sopath.c_str(), 1);
    setenv("basedir", basedir.c_str(), 1);
    // print_envp();

    if (argc > optind){
	int argv_arr_len = 0; 
	int argv_len = 0;
	int i = 0;
	char **exec_argv;
	argv_arr_len = argc - optind; 
	exec_argv = (char**)malloc(argv_arr_len*sizeof(char*)+1);
	while(optind < argc){
	    exec_argv[i++] = argv[optind++];
	}
	exec_argv[i] = NULL;

	string cmd_dir = "";
	string cmd = string(exec_argv[0]);
	const char* temp;
	char exec_pathname[MAX_CMD_BUF];
	if(is_cmd_exist_in_shell(cmd, cmd_dir)){
	    temp = (cmd_dir + cmd).c_str();
	}else{
	    temp = cmd.c_str();
	}
	strcpy(exec_pathname, temp);
	execve(exec_pathname, exec_argv, environ);
    }else{
        fprintf(stderr, "no command given.\n");
    } 
}
