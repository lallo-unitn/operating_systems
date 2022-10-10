#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <fcntl.h>

#define TRUE 1
#define FALSE 0

#define MAX_DEPTH 5
#define MAX_BROADTH 10
#define MAX_ARG 10

int procNumber[MAX_DEPTH] = {0};
pid_t children[MAX_BROADTH] = {0};
pid_t myParent = 0;
int idChild = 0;
int broadth = 0;
int depth = 0;

typedef struct msg{
    long mtype;
    char mtext[2];
} Msg_packet;

int createChild(int queue, int level, int* procNumber);
int killChildren(int queue, int level, int* procNumber);
int printTree(int queue, int* procNumber);
int quit(int queue, int* procNumber1);
void printTab();

int main(){
    char buf[MAX_ARG];
    int level = 0;

    creat("tmp/tree", 0777);
    key_t treeKey1 = ftok("/tmp/tree",1);

    //Remove queue if already exists
    int treeQueue1 = msgget(treeKey1, 0777 | IPC_CREAT);
    msgctl(treeQueue1,IPC_RMID,NULL);
    treeQueue1 = msgget(treeKey1,0777|IPC_CREAT);

    int queue; //will store the queue ID

    

    while(TRUE){
        sleep(1);
        printf("\nNext Command: ");
        fflush(stdout);

        //Fetch command from STDIN
        int r = read(STDIN_FILENO, buf, MAX_ARG-1);
        buf[r] = 0; //Terminate string

        int ind = 0;

        queue = treeQueue1;
        int * procNumber1 = procNumber;

        switch (buf[ind])
        {
        case 'c':
            level = atoi(buf + ind + 1);
            if(level == 1){
                if(broadth++ < MAX_BROADTH){
                    printf("Creating child at level %d\n", level);
                    procNumber[level]++;
                    idChild = fork();
                } else{
                    fprintf(stderr, "Error: too much children\n");
                }
            } else if(level > 1 && level <= MAX_DEPTH){
                printf("Creating grandchild at level %d", level);
                createChild(queue, level, procNumber);
            } else{
                fprintf(stderr, "Error: Invalid parameter1\n");
            }
            break;

        case 'k':
            level = atoi(buf + ind + 1);
            if(level > -1 && level <= MAX_DEPTH){
                killChildren(queue, level, procNumber);
            } else{
                fprintf(stderr, "Error: Invalid parameter2\n");
            }
            break;
        
        case 'p':
            printf("Printing process tree... \n");
            printTree(queue, procNumber);
            break;
        
        case 'q':
            printf("Terminating...\n");
            quit(queue, procNumber1);
            break;
        
        default:
            fprintf(stderr, "Error: Invalid parameter3\n");
            break;
        }

        if(idChild == 0){
            myParent = getppid();
            depth++;
            broadth = 0;
            printf("I'm new child at level %d with id = %d\n", level, getpid());
            break;
        } else{
            children[broadth - 1] = idChild;
        }
    }

    Msg_packet msgRcv;
    
    while(TRUE){
        msgrcv(queue, &msgRcv, sizeof(msgRcv.mtext), depth, 0);

        if(strcmp(msgRcv.mtext, "c") == 0){
            if(broadth++ < MAX_BROADTH){
                children[broadth - 1] = fork();
                if(children[broadth - 1] == 0){
                    depth++;
                    broadth = 0;
                    myParent = getppid();
                    printf("I'm new child at level %d with id = %d\n", level, getpid());
                }
            } else{
                fprintf(stderr, "Error: Too many children\n");
            }
        } else if(strcmp(msgRcv.mtext, "k") == 0){
            Msg_packet msgKill;
            strncpy(msgKill.mtext, "k", 2);
            msgKill.mtype = depth + 1;
            for(int i = 0; i < broadth; i++){
                msgsnd(queue, &msgKill, sizeof(msgKill.mtext), 0);
            }
            while(wait(NULL) > 0);
            printf("[%d] Terminating\n",getpid()); 
            exit(0);
        } else if(strcmp(msgRcv.mtext, "p") == 0){
            printTab();
            printf("[ID %d - Parent: %d] depth %d\n", getpid(), myParent, depth);
        } else{
            printf("[%d]Invalid message read '%s'\n",getpid(),msgRcv
            .mtext);
        }
        sleep(1); //Prevent process from reading additional messages
    }
}

int createChild(int queue, int level, int* procNumber){
    Msg_packet msgCreate;
    msgCreate.mtype = level - 1;
    strncpy(msgCreate.mtext, "c", 2);
    for(int i = 0; i < procNumber[level - 1]; i++){
        msgsnd(queue, &msgCreate, sizeof(msgCreate.mtext), 0);
        procNumber[level]++;
    }
    return 0;
}

int killChildren(int queue, int level, int*procNumber){
    Msg_packet msgKill;
    msgKill.mtype = level;
    strncpy(msgKill.mtext, "k", 2);
    for(int i = 0; i < procNumber[level]; i++){
        msgsnd(queue, &msgKill, sizeof(msgKill.mtext), 0);
    }

    return 0;
}

int printTree(int queue, int* procNumber){
    printTab();
    printf("[ID %ld - Parent: %ld] depth %d", (long)getpid(), (long)myParent, depth);
    Msg_packet msgPrint;
    strncpy(msgPrint.mtext, "p", 2);
    
    for(int i = 1; i <= MAX_DEPTH; i++){
        msgPrint.mtype = i;
        for(int j = 0; j < procNumber[i]; j++){
            msgsnd(queue, &msgPrint, sizeof(msgPrint.mtext), 0);
        }
    }
    return 0;
}

int quit(int queue, int* procNumber1){
    killChildren(queue, 1, procNumber1);
    printf("Sent quit message to all on %d. Waiting for their termination \n",queue);
    while(wait(NULL)>0);
    printf("Terminating program\n");
    exit(0);
    return 0;
}

void printTab(){
    for(int i = 1; i <= depth ; i++){
        printf("\t");
    }
}