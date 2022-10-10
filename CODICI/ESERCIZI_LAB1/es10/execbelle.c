#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h> 
#include <sys/types.h> 
#include <unistd.h>
#define EXE_NR 2
#define MAX_EXE_LEN 25
const char exes[EXE_NR][MAX_EXE_LEN] = {
    "./eseguibile1.sh",
    "./eseguibile2.sh"
};

int main(int argc, char **argv){
    int status;
    int i;
    pid_t child_pid;
    for(i = 0; i < EXE_NR; i++){
        child_pid = fork();
        if(child_pid == 0){
            execl(exes[i],exes[i], NULL);
            exit(EXIT_SUCCESS);
        } else {
            waitpid(child_pid, &status, 0);
            printf(" +: L'eseguibile del figlio con PID %d ha terminato con stato %d\n", (int)child_pid, WEXITSTATUS(status));
        }
    };
    return 0;
}



/*pid_t child_pid = fork();
    int status;
    if(child_pid == 0){ 
        execl("./eseguibile.sh","./eseguibile.sh", NULL);
        exit(EXIT_SUCCESS);
    } else {
        //wait(&status);
        waitpid(child_pid, &status, 0);
        printf(" +: L'eseguibile ha terminato con stato %d\n", WEXITSTATUS(status));
    };*/
    