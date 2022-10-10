#include <stdio.h>
#include <stdlib.h> 
#include <signal.h>

void myHandler(int sigNum){
    printf("CTRL+Z\n");
    exit(2);
}

void main(){
    signal(SIGCHLD, SIG_DFL);   //Use default handler

    /*
    struct sigaction sa;
    sa.sa_handler = SIG_DFL;
    sigaction(SIGTSTP, &sa, NULL);
    */

   while(1);
    
}