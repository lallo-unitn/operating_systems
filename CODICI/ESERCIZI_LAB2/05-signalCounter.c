#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_SENDERS 2

typedef enum{false, true} bool;

long int senderReg[MAX_SENDERS][3];

bool foreground(){
    bool fg = false;
    if(getpgrp() == tcgetpgrp(STDOUT_FILENO)) fg = true;
    return fg;
}

void counterHandler(int sig, siginfo_t *info, void *context){
    if(sig == SIGUSR1 || sig == SIGUSR2){
        bool flag = false;
        //printf("PID of signal sender = %d\n", info->si_pid);
        printf("%d %d", getpgrp(), tcgetpgrp(STDOUT_FILENO));
        if(getpgrp() == tcgetpgrp(STDOUT_FILENO))printf("Receiver [%ld] received signal [%d] from [%d]\n", (long)getpid(), sig, info->si_pid);
        for(int i = 0; i < MAX_SENDERS && !flag; i++){
            if(senderReg[i][0] == (long)(info->si_pid)){
                if(sig == SIGUSR1) senderReg[i][1]++;
                if(sig == SIGUSR2) senderReg[i][2]++;
                flag = true;
            } else if(senderReg[i][0] == -1){
                senderReg[i][0] = (long)(info->si_pid);
                senderReg[i][1] = 0;
                senderReg[i][2] = 0;
                if(sig == SIGUSR1) senderReg[i][1]++;
                if(sig == SIGUSR2) senderReg[i][2]++;
                flag = true;
            }
        }
    } else if(sig == SIGTERM || sig == SIGINT){
        printf("\nTerminating...\n");
        for(int i = 0; i < MAX_SENDERS && senderReg[i][0] != -1; i++){
            printf("Sender ID [%ld]. SIGUSR1 [%ld], SIGUSR2 [%ld]\n",senderReg[i][0],senderReg[i][1],senderReg[i][2]);
            fflush(stdout);
        }
        sleep(1);
        exit(0);
    }
}

int main(){
    for(int i = 0; i < MAX_SENDERS; i++){
        senderReg[i][0] = -1;
    }
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = counterHandler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);    
    sigaction(SIGINT, &sa, NULL);
    printf("[PID: %ld]\n", (long)getpid());
    fflush(stdout);
    while(true);
    return 0;
}