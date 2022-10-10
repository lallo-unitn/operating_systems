#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h> 

void myHandler(int sigNum){
    printf("Child terminated \n");
}
int main(){
    struct sigaction sa;
    sa.sa_handler = &myHandler;
    sigaction(SIGCHLD, &sa, NULL);
    pid_t child = fork();
    if(!child){
        return 0;
    }
    while(wait(NULL) > 0);
}