#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>

#define MAX_IN_LEN 10
#define MAX_TREE_DEPTH 5
#define MAX_CHILDREN 10

#define RED "\033[0;31m"
#define GREEN "\033[32m"
#define DF "\033[0m"

int createFlag = 0;
int isChild = 0;
pid_t myParent = 0;

int depth = 0;
int broadth = 0;

int br[MAX_TREE_DEPTH];

void signalIntHandlerMasterParent(int sig);
void signalPrintHandler(int sig);
void signalHandlerCreate(int sig);
int createChild(short level);
int killChildren(int level);
void quit(int sig);
void signalTermHandlerChild(int sig);
void perr(char * str);
void printTab();
void printTree();

int main(){
    char buf[MAX_IN_LEN];
    int level = 0;
    br[0] = getpid();

    for (int i = 1; i <= MAX_TREE_DEPTH; i++)
    {   
        br[i] = fork();
        if(br[i] == 0){
            break;          //Only the first process must go looping
        }
    }

    setpgid(0,getpid());    //set all the above children to group leader

    //Block all master parents
    if(br[0] != getpid()){
        signal(SIGTERM, SIG_IGN);
        signal(SIGINT, signalIntHandlerMasterParent);
        while(1);       // Block first process
    }

    printf("Master processes = ");
    for (int i = 0; i < MAX_TREE_DEPTH; i++)
    {
        printf("[ %d ]  ", br[i]);
    }
    fflush(stdout);

    signal(SIGUSR2, signalPrintHandler); 
    signal(SIGUSR1, signalHandlerCreate);
    signal(SIGINT, quit);

    while(1){
        sleep(1);
        printf("\nNext command: ");
        fflush(stdout);

        int r = read(STDIN_FILENO, buf, MAX_IN_LEN-1);
        buf[r] = 0;
    
    
        switch (buf[0])
        {
        case 'c':
            level = atoi(buf + 1);
            if(level == 1){
                broadth++;
                if(broadth < MAX_CHILDREN){
                    printf("Creating child at level %d\n", level);
                    isChild = !fork();
                } else{
                    perr("Too many children\n");
                }
            } else if(level > 1 & level <= MAX_CHILDREN){
                printf("Creating grandchild at level %d\n", level);
                createChild(level);
            } else{
                perr("Invalid parameter\n");
            }
            break;

            case 'k': //Kill child/grandchild
                level = atoi(buf+1); //Get level of child
                //Kill children by issuing the termination signal
                if(level > -1 && level <= MAX_TREE_DEPTH)
                    killChildren(level);
                else
                    perr("Invalid parameter\n");
            break;

            case 'p':
                printf("Printing Tree:\n");
                printTree();
            break;

            case 'q':
                printf("Terminating...\n");
                quit(0);
            break;

            default:
                perr("Invalid parameter\n");
        }

        if(isChild){
            signal(SIGTERM, signalTermHandlerChild);
            myParent = getppid(); //Get parent ID
            depth++;
            broadth = 0;    // Set broadth of own branch
            setpgid(0, br[depth]); // Join master parent's group
            printf("I'm new child at level %d with id = %d\n", depth, getpid());
            break; // REFERES TO WHILE LOOP
        }
    }
    while (1){
        if(createFlag){
            if(broadth++ < MAX_CHILDREN){
                if(!fork()){
                    depth++;
                    broadth = 0;
                    myParent = getppid();
                    setpgid(0, br[depth]);
                    printf("I'm new child at level %d with group = %d\n",depth,getpgid(0));
                }
            } else{
                perr("Too many children\n");
            }
            createFlag = 0;
        }
    }
    
    return 0;
}



void signalIntHandlerMasterParent(int sig){
    while(wait(NULL) > 0);
    printf("[ Terminating master parent ]");
    exit(0);
}

void signalPrintHandler(int sig){
    printTab();
    printf("%s[ ID %d - Parent: %d - Group %d ] depth %d%s\n", GREEN, getpid(), myParent, getpgid(0), depth, DF);
}

void signalHandlerCreate(int sig){
    createFlag = 1;
}

void quit(int sig){
    for (int i = 0; i <= MAX_TREE_DEPTH; i++)
    {
        kill(-br[i], SIGTERM);
        kill(-br[i], SIGINT);
    }
    sleep(1);
    while(wait(NULL) > 0);
    exit(0);
}

int createChild(short level){
    // Issue a SIGUSR1 to instruct the children at level to create a new child
    return kill(-br[level-1],SIGUSR1);
}

void signalTermHandlerChild(int sig){
    printf("[Child terminating] \n");
    exit(0);
}

void perr(char * str){
    fprintf(stderr,"%s%s%s",RED,str,DF);
}

void printTab(){
    for (int i = 1; i <= depth ; i++){
        printf("\t");
    }
}

int killChildren(int level){
    printf("Killing children at level %d\n",level);
    return kill(-br[level], SIGTERM);
}

void printTree(){
    for(int i = 0; i < MAX_TREE_DEPTH; i++){
        kill(-br[i], SIGUSR2);
    }
}