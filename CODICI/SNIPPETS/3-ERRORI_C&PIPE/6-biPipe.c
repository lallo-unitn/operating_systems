#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#define WRITE 1
#define READ 0

void main(){
    int pipe1[2];
    int pipe2[2];
    char buf[50];
    pipe(pipe1);
    pipe(pipe2);
    int p2 = !fork();
    if(p2){
        close(pipe1[WRITE]);
        close(pipe2[READ]);
        int r = read(pipe1[READ], &buf, 50);
        close(pipe1[READ]);
        printf("P2 received: '%s' \n", buf);
        write(pipe2[WRITE], "Msg from p2", 12);
        close(pipe2[WRITE]);
    } else{
        close(pipe1[READ]);
        close(pipe2[WRITE]);
        write(pipe1[WRITE], "Msg from p1", 12);
        close(pipe1[WRITE]);
        int r = read(pipe2[READ], &buf, 50);
        close(pipe2[READ]);
        printf("P1 received: '%s'\n", buf);
    }
    while(wait(NULL) > 0);
}