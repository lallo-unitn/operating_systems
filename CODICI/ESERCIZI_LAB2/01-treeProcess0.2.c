#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>

#define MAX_TREE_DEPTH 5
#define MAX_CHILDREN 10
#define MAX_INPUT 10

int br[MAX_TREE_DEPTH];

pid_t myParent;
int isChild = 0;
int createFlag = 0;
int depth = 0;
int broadth = 0;

int createChild(int level);
int killChildren(int level);
void printTree();
void printTab(int level);
void intMasterProcHandler(int sig);
void createHandler(int sig);
void termChildHandler(int sig);
void printInfoHandler(int sig);
void quit(int sig);

int main(){
    char buf[MAX_INPUT];
    int level = 0;

    br[0] = getpid();

    for(int i = 1; i <= MAX_TREE_DEPTH; i++){       // Genera master processes per livelli
        br[i] = fork();
        if(br[i] == 0){
            break;                                  // First process looping
        }
    }

    setpgid(0, getpid());

    if(br[0] != getpid()){                      // For all master processes
        signal(SIGTERM, SIG_IGN);               // First proc does not exe
        signal(SIGINT, intMasterProcHandler);
        while(1);
    }

    signal(SIGUSR1, createHandler);
    signal(SIGUSR2, printInfoHandler);
    signal(SIGTERM, termChildHandler);

    printf("Master processes = ");
    for(int i = 0; i < MAX_TREE_DEPTH; i++){
        printf("[ %d ] ", br[i]);
    }
    fflush(stdout);
    printf("\n");

    while(1){
        sleep(1);
        printf("\nNext command: ");
        fflush(stdout);
        int r = read(STDIN_FILENO, buf, MAX_INPUT);
        buf[r] = 0;

        switch (buf[0]){
        case 'c':
            level = atoi(buf + 1);
            if(level == 1){                          // Immediate child
                if(broadth++ < MAX_CHILDREN){
                    printf("Generating child at level %d\n", level);
                    isChild = !fork();
                } else{
                    fprintf(stderr, "Error: too much children\n");
                }
            } else if(level > 1 && level < MAX_TREE_DEPTH){
                printf("Generating grandchild at level %d\n", level);
                createChild(level);
            } else{
                fprintf(stderr, "Invalid paramenter\n");
            }
        break;

        case 'k':
            level = atoi(buf + 1);
            if(level > -1 && level <= MAX_TREE_DEPTH){
                killChildren(level);
            } else{
                fprintf(stderr, "Invalid paramenter\n");
            }
        break;

        case 'p':
            printf("Printing tree: \n");
            printTree();
        break;

        case 'q':
            printf("Terminating... \n");
            quit(0);
        break;
        
        default:
            fprintf(stderr, "Invalid paramenter\n");
        break;
        }

        if(isChild){                                // If first child
            signal(SIGTERM, termChildHandler);
            myParent = getppid();
            broadth = 0;
            depth++;
            setpgid(0, br[depth]);
            printf("DEBUG1\n");
            printf("I'm new child at level %d with id = %d\n", depth, getpid());
            break;
        }

    }

    while(1){                                       // Child processes looping
        if(createFlag){                             // Check if process must create child
            if(broadth++ < MAX_CHILDREN){
                if(!fork()){
                    signal(SIGTERM, termChildHandler);
                    broadth = 0;
                    myParent = getppid();
                    depth++;
                    setpgid(0, br[depth]);
                    printf("I'm new child at level %d with group = %d\n", depth, getpgid(0));
                }
            } else{
                fprintf(stderr, "Error: too much children\n");
            }
            createFlag = 0;
        }
    }

}


int createChild(int level){
    return kill(-br[level-1], SIGUSR1);
}

int killChildren(int level){
    printf("[Issuing termination signal at level %d]\n", level);
    return kill(-br[level], SIGTERM);
}

void printTree(){
    for(int i = 0; i <= MAX_TREE_DEPTH; i++){
        kill(-br[i], SIGUSR2);
    }
}

void printTab(){
    for(int i = 0; i < depth; i++){
        printf("\t");
    }
}

void intMasterProcHandler(int sig){
    while(wait(NULL) > 0);
    printf("[Terminating master parent]");
    exit(0);
}

void createHandler(int sig){
    createFlag = 1; 
}

void termChildHandler(int sig){
    printf("[Child terminating]\n");
    exit(0);
}

void printInfoHandler(int sig){
    printTab();
    printf("[ID %ld - Parent: %ld] depth %d\n", (long)getpid(), long(myParent), depth);
}

void quit(int signo){
    for(int i = 1; i <= MAX_TREE_DEPTH; i++){
        killChildren(i); // Terminate children
        kill(-br[i],SIGINT);  // Terminate master parent
    }
    sleep(1); // wait for all processes to be terminated
    while(wait(NULL)>0);
    exit(0);
}