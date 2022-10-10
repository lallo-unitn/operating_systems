#include <stdio.h>
#include <stdlib.h> 
#include <signal.h>

void myHandler(int sigNum){
    if(sigNum == SIGINT) printf("CTRL+C\n");
    else if(sigNum == SIGTSTP) printf("CTRL+Z\n");
    exit(2);
}

void main(){
    signal(SIGINT, myHandler);
    signal(SIGTSTP, myHandler);

    /*
    struct sigaction sa;
    sa.sa_handler = &myHandler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTSTP, &sa, NULL);
    */

   while(1);
    
}