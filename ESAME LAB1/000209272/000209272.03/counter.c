#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#define BUF_SIZE_CHAR 1

int countChar(char *fileName);
int countLines(char *fileName);
int countCharP();
int countLinesP();

int main(int argc, char **argv){
    int linesNr = 0;
    int charNr = 0;
    pid_t child_pid;
    int status;

    if(argc == 2){
        if(strcmp(argv[1], "-c") == 0 ){
            child_pid = fork();
            if(child_pid == 0){
                charNr = countCharP();
                printf("%d \n", charNr);
                exit(EXIT_SUCCESS);
            } else {
                while(waitpid(child_pid, &status, 0) > 0);
            }
        } else if(strcmp(argv[1], "-l") == 0 ){
            child_pid = fork();
            if(child_pid == 0){
                linesNr = countLinesP();
                printf("%d \n", linesNr);
                exit(EXIT_SUCCESS);
            } else {
                while(waitpid(child_pid, &status, 0) > 0);
            }
        } else {
            fprintf(stderr, "Comando non valido \n");
            return 1;
        }
    }

    if(argc == 3){
        if(strcmp(argv[1], "-c") == 0 && strcmp(argv[2], "-l") == 0){
            child_pid = fork();
            if(child_pid == 0){
                charNr = countCharP();
                linesNr = countLinesP();
                printf("%d %d \n", charNr, linesNr);
                exit(EXIT_SUCCESS);
            } else {
                while(waitpid(child_pid, &status, 0) > 0);
            }
        } else if(strcmp(argv[1], "-l") == 0 && strcmp(argv[2], "-c") == 0){
            child_pid = fork();
            if(child_pid == 0){
                charNr = countCharP();
                linesNr = countLinesP();
                printf("%d %d \n", linesNr, charNr);
                exit(EXIT_SUCCESS);
            } else {
                while(waitpid(child_pid, &status, 0) > 0);
            }
        } else if(strcmp(argv[1], "-l") == 0 && strcmp(argv[2], "-c") != 0){
            child_pid = fork();
            if(child_pid == 0){
                linesNr = countLines(argv[2]);
                printf("%d \n", linesNr);
                exit(EXIT_SUCCESS);
            } else {
                while(waitpid(child_pid, &status, 0) > 0);
            }
        } else if(strcmp(argv[1], "-c") == 0 && strcmp(argv[2], "-l") != 0){
            child_pid = fork();
            if(child_pid == 0){
                charNr = countChar(argv[2]);
                printf("%d \n", charNr);
                exit(EXIT_SUCCESS);
            } else {
                while(waitpid(child_pid, &status, 0) > 0);
            }
        } else {
            fprintf(stderr, "Comando non valido \n");
            return 1;
        }
    }

    if(argc == 4){
        if(strcmp(argv[1], "-l") == 0 && strcmp(argv[2], "-c") == 0){
            child_pid = fork();
            if(child_pid == 0){
                charNr = countChar(argv[3]);
                linesNr = countLines(argv[3]);
                printf("%d %d \n", linesNr, charNr);
                exit(EXIT_SUCCESS);
            } else {
                while(waitpid(child_pid, &status, 0) > 0);
            }
        } else if(strcmp(argv[1], "-c") == 0 && strcmp(argv[2], "-l") == 0){
            child_pid = fork();
            if(child_pid == 0){
                charNr = countChar(argv[3]);
                linesNr = countLines(argv[3]);
                printf("%d %d \n", charNr, linesNr);
            } else {
                while(waitpid(child_pid, &status, 0) > 0);
            }
        } else {
            fprintf(stderr, "Comando non valido \n");
            return 1;
        }
    }

/*
    if(strcmp(argv[1], "-c") == 0){
        if(strcmp(argv[2], "-l")  == 0){
            charNr = countChar(argv[3]);
            linesNr = countLines(argv[3]);
            printf("%d %d \n", charNr, linesNr);
        } else {
            charNr = countChar(argv[2]);
            printf("%d \n", charNr);
        }
    
    } else if(strcmp(argv[1], "-l") == 0){
        if(strcmp(argv[2], "-c") == 0){
            linesNr = countLines(argv[3]);
            charNr = countChar(argv[3]);
            printf("%d %d \n", linesNr, charNr);
        }else {
            printf("%s \n", argv[2]);
            linesNr = countLines(argv[2]);
            printf("%d \n", linesNr);
        }
    
    } else {
        fprintf(stderr, "Comando non valido \n");
        return 1;
    }
    */
    return 0;
}

int countChar(char *fileName){
    char buf [BUF_SIZE_CHAR];
    int count = 0;
    int fd = open(fileName, O_RDONLY | O_CREAT);
    if(fd < 0){                                //FD ERROR
        fprintf(stderr, "Impossibile aprire il file descriptor \n");
        exit(EXIT_FAILURE);
    }
    while(read(fd, buf, BUF_SIZE_CHAR) != 0){
        count++;
    }
    close(fd);
    return count;
}

int countLines(char *fileName){
    char buf [BUF_SIZE_CHAR];
    int count = 0;
    int fd = open(fileName, O_RDONLY | O_CREAT);
    if(fd < 0){                                //FD ERROR
        fprintf(stderr, "Impossibile aprire il file descriptor \n");
        exit(EXIT_FAILURE);
    }
    while(read(fd, buf, BUF_SIZE_CHAR) != 0){
        if(strcmp(buf, "\n")){
            count++;
        }
    }
    close(fd);
    return count;
}

int countCharP(){
    char buf [BUF_SIZE_CHAR];
    int count = 0;
    while(read(STDIN_FILENO, buf, BUF_SIZE_CHAR) != 0){
        count++;
    }
    return count;
}

int countLinesP(){
    char buf [BUF_SIZE_CHAR];
    int count = 0;
    while(read(STDIN_FILENO, buf, BUF_SIZE_CHAR) != 0){
        if(strcmp(buf, "\n")){
            count++;
        }
    }
    return count;
}

