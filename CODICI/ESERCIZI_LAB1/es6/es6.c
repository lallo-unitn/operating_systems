#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define N_CMD 3
#define MAX_CMD_LENGTH 256

int main(int argc, char **argv){
    const char VAL_CMD[N_CMD][MAX_CMD_LENGTH] = {
        "cmd1",
        "cmd2",
        "cmd3"
    };
    int i;
    char **found_cmd = (char **)malloc(argc * sizeof(char*));
    for(i = 0; i < argc; i++) found_cmd[i] = (char *)malloc(MAX_CMD_LENGTH * sizeof(char));
    int j;
    int is_valid;
    int count = 0;

    for(i = 1; i < argc; i++){
        is_valid = false;
        
        for(j = 0; j < N_CMD && is_valid == 0; j++){
            if(strcmp(VAL_CMD[j], argv[i]) == 0){
                is_valid = true;
                strcpy(found_cmd[count], argv[i]);
                count++;
                printf("%d\n", count);
            }
        }
    }
    for(i = 0; i < argc; i++) free(found_cmd[i]);
    free(found_cmd);
    return 0;
}