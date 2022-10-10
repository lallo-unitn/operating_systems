#include <stdio.h>
#include <errno.h>
#include <string.h>

extern int errno;
void main(){
    FILE *pf;
    pf = fopen("notExistingFile.boh", "rb");
    if(pf == NULL){
        fprintf(stderr, "errno = %d\n", errno);
        perror("Error printed by perror");
        fprintf(stderr, "Strerror: %s\n", strerror(errno));
    } else{
        fclose(pf);
    }
}