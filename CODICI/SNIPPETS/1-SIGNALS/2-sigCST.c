#include <stdio.h>
#include <stdlib.h> 
#include <signal.h>

void myHandler(int sigNum){
    printf("CTRL+Z\n");
    exit(2);
}

void main(){
    signal(SIGINT, SIG_IGN);    //Ignore signal
    signal(SIGCHLD, SIG_DFL);   //Use default handler
    signal(SIGTSTP, myHandler); //Use myHandler

    /*
    struct sigaction sa;
    sa.sa_handler = &myHandler;
    sigaction(SIGTSTP, &sa, NULL);
    */

   while(1);
    
}