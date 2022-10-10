#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#define MAX_FILES 10
#define BUF_SIZE 1

int is_valid_char(char check);

int main(int argc, char **argv){
    if(argc > MAX_FILES + 1 || argc < 2){
        fprintf(stderr, " ?: 1 < nargs < 11 expected \n");
        exit(EXIT_FAILURE);
    }
    pid_t child_pid;
    int status;
    int i;
    int fd;
    int shm_fd = open("firstchar_tmp", O_WRONLY | O_CREAT | O_TRUNC);
    if(shm_fd < 0){
        fprintf(stderr, " ?: Couldn't open file descriptor");
        exit(EXIT_FAILURE);
    }
    //char* output = (char*) malloc((argc-1)*sizeof(char));
    char output[50] = "";
    char buf [BUF_SIZE];
    int flag = 0;
    for(i = 1; i < argc && flag == 0; i++){
        child_pid = fork();
        if(child_pid == 0){                            //CHILD
            fd = open(argv[i], O_RDONLY | O_CREAT);
            if(fd < 0){                                //FD ERROR
                lseek(shm_fd, 0, SEEK_SET);
                write(shm_fd, " ?: Couldn't open file descriptor", strlen(" ?: Couldn't open file descriptor"));
                exit(EXIT_FAILURE);
            }
            read(fd, buf, BUF_SIZE);
            if(!is_valid_char(buf[0])){                //CHAR NOT VALID
                buf[0] = '-';
            }
            write(shm_fd, buf, BUF_SIZE);
            exit(EXIT_SUCCESS);
        } else {                                       //PARENT
            while(waitpid(child_pid, &status, 0) > 0);
            if(WEXITSTATUS(status) == EXIT_FAILURE){
                flag = 1;
            } else if(WEXITSTATUS(status) == EXIT_SUCCESS) {
                close(fd);
            } else {
                fprintf(stderr, " ?: Unknown error");
                lseek(shm_fd, 0, SEEK_SET);
                write(shm_fd, " ?: Unknown error", strlen(" ?: Unknown error"));
            }
        }
    }
    close(shm_fd);
    shm_fd = open("firstchar_tmp", O_RDONLY);
    read(shm_fd, output, argc - 1);
    if(flag == 0){
        printf("%s", output);
        printf("\n");
    } else {
        fprintf(stderr, "%s", output);
        fprintf(stderr, "\n");
        exit(EXIT_FAILURE);
    }
    close(shm_fd);
    return 0;
}

int is_valid_char(char check){
    if((check >= 'A' && check <= 'z') || (check >= '0' && check <= '9')){
        return 1;
    } else {
        return 0;
    }
}