#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void myHandler(int sigNum){
    printf("[%d]ALARM!\n",getpid());
}

int main(){
    struct sigaction sa;
    sa.sa_handler = &myHandler;
    sigaction(SIGALRM, &sa, NULL);
    pid_t child = fork();
    if(!child) while(1);
    printf("[%ld] sending ALARM to %ld in 1 s\n", (long) getpid(), (long) child);
    sleep(1);
    kill(child, SIGALRM);
    printf("[%ld] sending SIGTERM to %ld in 1 s\n", (long) getpid(), (long) child);
    sleep(1);
    kill(child, SIGTERM);
    while(wait(NULL) > 0);
    return 0;
}