#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>

void myHandler(int sig){
    printf("[%ld, %ld] sig %d received\n", getpid(), getpgid(0), sig);
    sleep(1);
    exit(0);
}

void main(){
    struct signature sa;
    sa.sa_handler = myHandler;
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    int ancestor = getpid();
    int group1 = fork();
    int group2;
    if(getpid() != ancestor){   // If first child
        setpgid(0, getpid());   // Become group leader
        fork();
        fork();     // Generate four children
    }else {
        group2 = fork();
        if(getpid() != ancestor){   //If second child
            setpgid(0, getpid());   //Become group leader
            fork();
            fork(); //Generate four child in new group
        }
    }
    if(getpid() == ancestor){
        printf("[%ld] Ancestor sending signals\n", getpid());
        sleep(1);
        kill(-group, SIGUSR1);
        kill(-group, SIGUSR2);
    } else{
        printf("[%ld, %ld] child waiting for signal\n", getpid(), getpgid(0));
        while(1);
    }
    while(wait(NULL) > 0);
    printf("All children terminated\n");
}