#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h> 
#include <string.h>
#define CMD_NR 2

int main(int argc, char **argv){
    if(argc != CMD_NR + 1){
        printf(" -: Richiesti 2 argomenti");
        exit(EXIT_FAILURE);
    };
    int i;
    int status;
    pid_t child_pid;
   
    for(i = 1; i <= CMD_NR; i++){
        child_pid = fork();
        if(child_pid == 0){

            if(i == 1){
                system(strcat(argv[i], " > out.txt"));
            }
            else if(i == 2){
                system(strcat(argv[i], " out.txt"));
            };

            exit(EXIT_SUCCESS);
            
        } else {
            waitpid(child_pid, &status, 0);
        }
    }
    return 0;
    
}
