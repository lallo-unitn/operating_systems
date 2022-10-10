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

#define MAX_ARG 10
#define MAX_DEPTH 5
#define MAX_BROADTH 10
#define RED "\033[0;31m"
#define GREEN "\033[32m"
#define DF "\033[0m"

int createFlag=0;
int idChild = 0;
pid_t myParent = 0;

int depth = 0, broadth = 0;
int treeStruct1[MAX_DEPTH]; //Keep track of how many children are at each level
pid_t children[MAX_BROADTH]; //Store pid of children

int treeStruct2[MAX_DEPTH]; //Keep track of how many children are at each level

//Message struct
typedef struct msg{
    long mtype;
    char mtext[2];
} Msg_packet;


// Error printing function
void perr(char * str){
    fprintf(stderr,"%s%s%s",RED,str,DF);
}

//Tabulation printing function
void printTab(){
    for(int i = 1; i <= depth ; i++){
        printf("\t");
    }
}

//Child creation function
int createChild(int queue,int level,int * treeStruct){
    //Create Creation message
    Msg_packet msgCreate;
    //The creation of children at level N must be initiated by children at level N-1
    msgCreate.mtype=level-1; 
    strncpy(msgCreate.mtext,"c",2);

    //Send enough messages for all children
    for(int j = 0; j < treeStruct[level-1];j++){
        msgsnd(queue,&msgCreate,sizeof(msgCreate.mtext),0);
        //Update tree struct
        treeStruct[level]++;
    }
    return 0;
}

//Print tree
int printTree(int queue,int * treeStruct){
    printTab();
    printf("%s[ID %d - Parent: %d] depth %d%s\n",GREEN,getpid(),myParent,depth,DF);
    
    //Define print message
    Msg_packet msgPrint;
    strncpy(msgPrint.mtext,"p",2);

    //Send enough messages for every children
    for(int i = 1; i<=MAX_DEPTH;i++){
        //Send message to each depth
        msgPrint.mtype=i;
        for(int j = 0; j < treeStruct[i];j++){
            msgsnd(queue,&msgPrint,sizeof(msgPrint.mtext),0);
        }
    }
    return 0;
}

//Kill children
int killChildren(int queue, int level, int * treeStruct){
    Msg_packet msgKill;
    msgKill.mtype=level;
    strncpy(msgKill.mtext,"k",2);
    
    //Send message to each children of that level which, in turn, will notify his childrens
    for(int j = 0; j < treeStruct[level];j++){
        msgsnd(queue,&msgKill,sizeof(msgKill.mtext),0);
    }
    
    return 0;
}


//Quit function
void quit(int queue, int * treeStruct){
    killChildren(queue,1,treeStruct);
    printf("%sSent quit message to all on %d. Waiting for their termination%s\n",GREEN,queue,DF);
    while(wait(NULL)>0);
    printf("Terminating program\n");
    exit(0);
}

int main(){
    char buffer[MAX_ARG]; //Buffer for input parameters
    int level = 0; //Tmp variable

    //Create file for queue
    creat("/tmp/tree1", 0777);
    key_t treeKey1 = ftok("/tmp/tree1",1);

    creat("/tmp/tree2", 0777);
    key_t treeKey2 = ftok("/tmp/tree2",1);
    
    //Remove queue if already exists
    int treeeQueue1 = msgget(treeKey1,0777|IPC_CREAT);
    msgctl(treeeQueue1,IPC_RMID,NULL);
    treeeQueue1 = msgget(treeKey1,0777|IPC_CREAT);

    int treeeQueue2 = msgget(treeKey2,0777|IPC_CREAT);
    msgctl(treeeQueue2,IPC_RMID,NULL);
    treeeQueue2 = msgget(treeKey2,0777|IPC_CREAT);

    int queue; //will store the queue ID
    int queue2;

    if(buffer[0] == '1'){
        queue=treeeQueue1;
        treeStruct = treeStruct1;
        printf("Queue1 %d selected\n",queue);
    }else if(buffer[0] == '2'){
        queue=treeeQueue2;
        treeStruct = treeStruct2;
        printf("Queue2 %d selected\n",queue);
    }else if(buffer[0] == 'q'){
        quit(treeeQueue1,treeStruct1,treeeQueue2,treeStruct2);
    }else{
        perr("Invalid parameter\n");
        continue;
    }

    //Enter infinite loops for parsing commands
    while(1){
        sleep(1); //Avoid graphic glithces
        printf("\nNext command: ");fflush(stdout);

        //Fetch command from STDIN
        int r = read(STDIN_FILENO, buffer, MAX_ARG-1);
        buffer[r] = 0; //Terminate string
        
        int ind = 1; //tmp variable (USEFUL FOR NEXT UPDATE - see 03-queueTree2.c)

        //Variable holders for queue and tree structure
        queue = treeeQueue1;
        int * treeStruct = treeStruct1;

        queue2 = treeeQueue2;
        int * treeStruct02 = treeStruct2;

        //Parse command
        switch(buffer[ind]){
            case 'c': //Create child
                level = atoi(buffer+ind+1); //Get level of new child to be created
                if(level == 1){ //Create immediate child if space is available
                    if(broadth++ < MAX_BROADTH){
                        printf("Creating child at level %d, in tree: %c\n", level, buffer[0]);
                        if(buffer[0] == '1'){
                            treeStruct[level]++; //Update tree structure
                        } else if(buffer[0] == '2'){
                            treeStruct2[level]++; //Update tree structure
                        }else{
                            perr("Invalid parameter\n");
                        }
                        idChild = fork();
                    }else{  
                        perr("Too many children\n");
                    }
                }else if(level > 1 && level <= MAX_DEPTH){ //Issue creation of grandchild
                    printf("Creating grandchild at level %d, in tree: %c\n", level, buffer[0]);
                    if(buffer[0] == '1'){
                        createChild(queue,level,treeStruct);
                    } else if(buffer[0] == '2'){
                        createChild(queue2,level,treeStruct2);
                    }else{
                    perr("Invalid parameter\n");
                    }
                }else{
                    perr("Invalid parameter\n");
                }
                break;
            case 'k': //Kill child/grandchild
                level = atoi(buffer+ind+1); //Get level of child
                
                //Kill children
                if(buffer[0] == '1'){
                    if(level > -1 && level <= MAX_DEPTH){
                        killChildren(queue,level,treeStruct);
                    } else{
                    perr("Invalid parameter\n");
                    }
                }else if(buffer[0] == '2'){
                    if(level > -1 && level <= MAX_DEPTH){
                        killChildren(queue2,level,treeStruct2);
                    } else{
                    perr("Invalid parameter\n");
                    }
                }
                break;
            case 'p':
                printf("Printing Tree nr %c:\n", buffer[0]);
                if(buffer[0] == '1'){
                    printTree(queue,treeStruct);
                }
                else if(buffer[0] == '2'){
                    printTree(queue2,treeStruct2);
                }
                break;
            case 'q':
                if(buffer[0] == '1'){
                    quit(treeeQueue1,treeStruct1);
                } else if(buffer[0] == '1'){
                    quit(treeeQueue2,treeStruct02);
                }
            default:
                perr("Invalid parameter\n");
        }
        if(idChild == 0){ //If it's a level 1 child
            myParent = getppid(); //Keep track of who is the parent
            depth++; //Increase depth at which the child is 
            broadth = 0; // Set broadth of own branch to 0
            printf("I'm new child at level %d with id = %d in tree nr %c\n",depth,getpid(),buffer[0]);
            break; //Exit while loop to process messages
        }else{
            children[broadth-1] = idChild; //Keep track of children ID
        }
    }
    //Child waiting for messages
    Msg_packet msgRcv;
    while(1){
        //Hang for next message at a specific depth (message type)
        msgrcv(queue,&msgRcv,sizeof(msgRcv.mtext),depth,0);

        //Check message text
        if(strcmp(msgRcv.mtext,"c")==0){ //Create child
            if(broadth++ < MAX_BROADTH){ //Create new child if allowed
                children[broadth-1] = fork(); // Keep track of child id
                if (children[broadth-1] == 0){ //Child that is created
                    depth++; //Increase own depth
                    broadth=0; //Initialise broadth
                    myParent = getppid(); //Keep track of parent
                    printf("I'm new child at level %d with id = %d\n",depth,getpid());
                }
            }else{
                perr("Too many children\n");
            }
        }else if(strcmp(msgRcv.mtext,"k")==0){ //Kill
            //Send termination enough termination message (for each child)
            Msg_packet msgKill;
            //Send message at a further depth
            msgKill.mtype=depth+1;
            strncpy(msgKill.mtext,"k",2);

            // Send enough messages (NB: these messages might be read by other non children --> 
            // not a problem since other messages will be received )
            // i.e NUMBER of all sent messages == NUMBER of all children 
            for(int i = 0; i<broadth; i++){
                msgsnd(queue,&msgKill,sizeof(msgKill.mtext),0);
            }
            while(wait(NULL)>0); //Wait for the recursion to end
            printf("[%d] Terminating\n",getpid()); 
            exit(0);
        }else if(strcmp(msgRcv.mtext,"p")==0){ //Print
            printTab();
            printf("%s[ID %d - Parent: %d] depth %d%s\n",GREEN,getpid(),myParent,depth,DF);
        }else{
            printf("[%d]Invalid message read '%s'\n",getpid(),msgRcv
            .mtext);
        }
        sleep(1); //Prevent process from reading additional messages
    }
    
}