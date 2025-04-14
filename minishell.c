/*
Name: Lucas Vanzelli  
Pledge: I pledge my honor that I have abided by the Stevens Honor System.
HW 4 - minishell.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>

#define BLUE "\x1b[34;1m"
#define DEFAULT "\x1b[0m"

volatile sig_atomic_t interrupted = 0;

void handle_sigint(int sig){
    interrupted = 1;
}

//helper to sort pid ID numbers
int sorting_pids(const void *a, const void *b){
    return (*(int *) a - *(int *) b);
}

int main(){
    //signal handling
    struct sigaction signal;
    signal.sa_handler = handle_sigint;
    sigemptyset(&signal.sa_mask);
    signal.sa_flags = 0;
    if (sigaction(SIGINT, &signal, NULL) == -1) {
        fprintf(stderr, "Error: Cannot register signal handler. %s.\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    char input[1024];

    //if signal is interupted, let's ignore it!
    while (1){ 
        if(interrupted){
            printf("\n");
            interrupted = 0;
            continue;
        }

        //print current working directory
        char cwd[1024];
        if(getcwd(cwd, sizeof(cwd)) != NULL){
            printf("%s[%s]> %s", BLUE, cwd, DEFAULT);
        }else{
            fprintf(stderr, "Error: Cannot retrieve current working directory. %s.\n", strerror(errno));
        }

        //getting inputs
        if(fgets(input, sizeof(input), stdin) == NULL){
            if(errno == EINTR){
                continue;
            }
            fprintf(stderr, "Error: Failed to read from stdin. %s.\n", strerror(errno));
            break;
        }

        //terminate da string
        char *newline = strchr(input, '\n');
        if (newline) *newline = '\0';

        char *args[100];
        int i = 0;
        char *argument = strtok(input, " ");
        while(argument != NULL && i < 99){
            args[i++] = argument;
            argument = strtok(NULL, " "); //seperarte by space
        }
        args[i] = NULL; //contains the arguments

        if(args[0] == NULL){
            continue;
        }

        //exit
        if(strcmp(args[0], "exit") == 0){
            break;
        } 
        //cd
        else if(strcmp(args[0], "cd") == 0){
            if(args[1] && args[2]) {
                fprintf(stderr, "Error: Too many arguments to cd.\n");
            } 
            else if(args[1] == NULL || strcmp(args[1], "~") == 0){ //go to home directory
                struct passwd *pw = getpwuid(getuid());
                if (pw == NULL) {
                    fprintf(stderr, "Error: Cannot get passwd entry. %s.\n", strerror(errno));
                } else if (chdir(pw->pw_dir) != 0) {
                    fprintf(stderr, "Error: Cannot change directory to %s. %s.\n", pw->pw_dir, strerror(errno));
                }
            }else{
                if(chdir(args[1]) != 0){ //go to inpput directory
                    fprintf(stderr, "Error: Cannot change directory to %s. %s.\n", args[1], strerror(errno));
                }
            }
        } 
        else if(strcmp(args[0], "pwd") == 0){ //print current directory
            if(getcwd(cwd, sizeof(cwd)) != NULL){
                printf("%s\n", cwd);
            }else{
                fprintf(stderr, "Error: Cannot get current working directory. %s.\n", strerror(errno));
            }
        }
        else if(strcmp(args[0], "lf") == 0){ //list files
            //open cwd
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
            DIR *directory = opendir(cwd);
            if(!directory){
                fprintf(stderr, "Error: Cannot open current directory. %s.\n", strerror(errno));
                continue;
            }

            //iterate through current working directory
            struct dirent *inputs;
            while((inputs = readdir(directory)) != NULL){
                if(strcmp(inputs->d_name, ".") == 0 || strcmp(inputs->d_name, "..") == 0) //will not print "." or ".." files
                    continue;
                printf("%s\n", inputs->d_name);
            }
            closedir(directory);
            } 
        }
        else if (strcmp(args[0], "lp") == 0){
            //open proc directory
            DIR *proc = opendir("/proc");
            if(!proc){
                fprintf(stderr, "Error: Cannot open /proc. %s.\n", strerror(errno));
                continue;
            }

            struct dirent *inputs;
            int pid_numbers[4096];
            int count = 0;

            while((inputs = readdir(proc)) != NULL){
                if (inputs->d_name[0] < '0' || inputs->d_name[0] > '9') {
                    continue;
                }        
                //string to int
                pid_numbers[count++] = atoi(inputs->d_name);
            }
            closedir(proc);

            //sort the pids
            qsort(pid_numbers, count, sizeof(int), sorting_pids);

            //for each pid
            for (int i = 0; i < count; i++){
                int pid = pid_numbers[i];
                char path[256];
                char command[1024];
                char user[64] = "unknown";
                FILE *fp;
                struct stat info;

                //getting da user
                snprintf(path, sizeof(path), "/proc/%d", pid);

                if(stat(path, &info) == 0){
                    struct passwd *pw = getpwuid(info.st_uid);
                    if(pw){
                        snprintf(user, sizeof(user), "%s", pw->pw_name);
                    } 
                }

                snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);
                fp = fopen(path, "r");
                if(fp != NULL){
                    size_t num = fread(command, sizeof(char), sizeof(command) - 1, fp);
                    fclose(fp);
                    if(num > 0){
                        command[num] = '\0';
                        for (size_t j = 0; j < num; j++){
                            if (command[j] == '\0'){
                                command[j] = ' ';
                            }
                        }
                    }
                }

                //<PID ID> <USER> <COMMAND>
                printf("%d %s %s\n", pid, user, command);
            }        
        }else{
            pid_t pid = fork();
            if(pid == 0){
                execvp(args[0], args);
                fprintf(stderr, "Error: exec() failed. %s.\n", strerror(errno));
                exit(EXIT_FAILURE);
            }else if (pid > 0){
                int status;
                if(wait(&status) == -1){
                    fprintf(stderr, "Error: wait() failed. %s.\n", strerror(errno));
                }
            }else{
                fprintf(stderr, "Error: fork() failed. %s.\n", strerror(errno));
            }
        }
    }
    return 0;
}
