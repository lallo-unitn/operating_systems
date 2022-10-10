// compile, execute, and on new window => kill -14 <PID>

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void myHandler(int sig){
    printf("[%ld] ALARM!\n", (long) getpid());
    exit(0);
}

int main(){
    struct sigaction sa;
    sa.sa_handler = &myHandler;
    sigaction(SIGALRM, &sa, NULL);
    printf("I am %ld\n", (long) getpid());
    while(1);
    return 0;
}