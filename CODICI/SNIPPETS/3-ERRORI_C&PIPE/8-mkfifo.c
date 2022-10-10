#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

void main(){
    char *fifoName = "/tmp/fifo";
    mkfifo(fifoName, S_IRUSR | S_IRUSR);
    perror("Created?");
    if(fork() == 0){
        open(fifoName, O_RDONLY);
        printf("Open read\n");
    } else{
        sleep(1);
        open(fifoName, O_WRONLY);
        printf("Open write\n");
    }
}