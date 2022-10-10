#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

extern int errno;
void main(){
    if(kill(3443,SIGUSR1) == -1){
        fprintf(stderr, "errno = %d\n", errno);
        perror("Error printed by perror");
        fprintf(stderr, "Strerror: %s\n", strerror(errno));
    } else{
        printf("Signal sent\n");
    }
}