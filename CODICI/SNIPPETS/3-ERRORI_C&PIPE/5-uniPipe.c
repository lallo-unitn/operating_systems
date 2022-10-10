#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#define WRITE 1
#define READ 0

void main(){
    int fd[2];
    char buf[50];
    pipe(fd);
    int p2 = !fork();
    if(p2){
        close(fd[WRITE]);
        int r = read(fd[READ], &buf, 50);
        close(fd[READ]);
        printf("Buf: '%s'\n", buf);
    } else{
        close(fd[READ]);
        write(fd[WRITE], "writing", 8);
        close(fd[WRITE]);
    }
    while(wait(NULL) > 0);
}