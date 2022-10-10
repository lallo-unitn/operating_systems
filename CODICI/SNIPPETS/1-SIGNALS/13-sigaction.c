#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void myHandler(int sig){
    printf("Signal received\n");
}

int main(){
    struct sigaction sa;
    sa.sa_handler = myHandler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);
    kill(getpid(), SIGUSR1);
    return 0;
}