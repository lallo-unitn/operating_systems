#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

short cnt = 0;

void myHandler(int sig){
    printf("ALARM!\n");
    cnt++;
}

int main(){
    struct sigaction sa;
    sa.sa_handler = &myHandler;
    alarm(5);
    printf("Seconds remaining to previous alarm %d\n", alarm(2));
    while(cnt < 1);
}