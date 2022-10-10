#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

void main(){
    int group1 = fork();
    int group2;
    if (group1 == 0){               // Firdt child
        setpgid(0, getpid());       // Become group leader
        fork();
        fork();                     // Generate four children in new group
        sleep(2);
        return;
    } else{
        group2 = fork();
        if(group2 == 0){            // Firdt child
            setpgid(0, getpid());   // Become group leader
            fork();
            fork();                 // Generate four children in new group
            sleep(4);
            return;
        }
    }
    sleep(1);
    while(waitpid(-group1, NULL, 0) > 0);
    printf("Children in [%d] terminated\n", group1);
    while(waitpid(-group2, NULL, 0) > 0);
    printf("Children in [%d] terminated\n", group2);
}