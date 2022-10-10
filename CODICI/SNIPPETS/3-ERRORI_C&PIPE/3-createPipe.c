#include <stdio.h>
#include <unistd.h>
#define WRITE 1
#define READ 0

void main(){
    int fd[2];
    int esito = pipe(fd);   //Creating unnamed pipe
    if(esito == 0){
        write(f[WRITE], "writing", 8);  // Write to pipe using fd[1]
        char buf[50];
        read(fd[READ], &buf, 50);       // Read from pipe using fd[0]
        printf("Read '%s'\n", buf);
    }
}