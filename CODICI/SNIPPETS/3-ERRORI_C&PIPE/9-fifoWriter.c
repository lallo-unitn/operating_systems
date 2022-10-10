#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

void main(int argc, char *argv[]){
    int fd;
    char *fifoName = "/tmp/fifo1";
    char str1[80];
    char *str2 = "I'm a writer";
    mkfifo(fifoName, S_IRUSR | S_IWUSR);
    perror("Created?");
    fd = open(fifoName, O_WRONLY);
    write(fd, str2, strlen(str2) + 1);
    close(fd);
    fd = open(fifoName, O_RDONLY);
    read(fd, str1, sizeof(str1));
    printf("Reader is writing: %s\n", str1);
    close(fd);
}