#include <stdio.h>
#include <stdlib.h> 
#include <signal.h>

void myHandler(int sigNum){}

void main(){

    printf("DFL: %p\n", signal(SIGINT, SIG_IGN));
    printf("IGN: %p\n", signal(SIGINT, myHandler));
    printf("Custom: %p == %p\n", signal(SIGINT, SIG_DFL), myHandler);
       
}