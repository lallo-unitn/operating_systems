#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

void main(){
    int isChild = !fork();  // New child
    printf("PID %ld GID %ld\n", (long) getpid(), (long) getpgid(0));
    if(isChild){
        isChild = !fork();  // New child
        if(!isChild) setpgid(0, getpid());  // Become group  leader
        fork();
        printf("PID %ld GID %ld\n", (long) getpid(), (long) getpgid(0));
    }
    while(wait(NULL) > 0);
}