// compile, execute, and on new window => kill -18 or -10 <PID>

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void myHandler(int sig){
    printf("Continue!\n");
}

int main(){
    struct sigaction sa;
    sa.sa_handler = &myHandler;
    sigaction(SIGCONT, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
    printf("I am %ld\n", (long) getpid());
    pause();
    return 0;
}