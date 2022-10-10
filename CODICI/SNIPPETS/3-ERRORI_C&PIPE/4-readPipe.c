#include <stdio.h>
#include <unistd.h>
#define WRITE 1
#define READ 0

void main(){
    int fd[2];
    char buf[50];
    int esito = pipe(fd);   //Create unnamed pipe
    if(esito == 0){
        write(fd[WRITE], "writing", 8);
        int r = read(fd[READ], &buf, 50);
        printf("Last read %d. Received: '%s'\n", r, buf);
        close(fd[WRITE]);    // Hangs when commented
        r = read(fd[READ], &buf, 50);
        printf("Last read %d. Received: '%s'\n", r, buf);
    }
}