#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int i = 0;

void myHandler(int sig){
    printf("Signal received\n");
    i++;
}

int main(){
    sigset_t mod;
    //sigset_t old;
    printf("my id = %ld\n", (long) getpid());
    struct sigaction sa;
    sa.sa_handler = &myHandler;
    sigaction(SIGUSR1, &sa, NULL);
    //signal(SIGUSR1, myHandler);
    sigemptyset(&mod);
    sigaddset(&mod, SIGUSR1);
    while(1){
        if(i == 1){
            sigprocmask(SIG_BLOCK, &mod, NULL);
        }
    }
    return 0;
}