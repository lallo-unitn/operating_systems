#include <stdio.h>
#include <stdlib.h> 
#include <signal.h>

void myHandler(int sigNum){
    printf("CTRL+Z\n");
    exit(2);
}

void main(){
    signal(SIGCHLD, SIG_IGN);   //Ignore

    /*
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigaction(SIGTSTP, &sa, NULL);
    */

   while(1);
    
}