#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#define MAX_BUF 100

#define TRUE 1 
#define FALSE 0

#define MAX_SLAVES 10
#define PIPE_SLAVE "/tmp/fifoSlave"
#define PIPE_MASTER "/tmp/fifoMaster"

char fifoMaster[MAX_SLAVES][100];
char fifoMasterFD[MAX_SLAVES];

int main(int argc, char **argv){

    char tmpArg[10],tmpArg2[10];
    int slavesNr = atoi(argv[1]);
    
    printf("MASTER PID: [%d]\n", getpid());

    printf("___________CREATING FIFO MASTER -> SLAVES___________\n");

    for (int i = 1; i <= slavesNr; i++)
    {   
        strcpy(fifoMaster[i], PIPE_MASTER);
        sprintf(tmpArg, "%d", i);
        mkfifo(strcat(fifoMaster[i], tmpArg), S_IRUSR | S_IWUSR); // Creating FIFO with index i
        perror("Created?");
        printf("%s\n", fifoMaster[i]);
    }

    printf("\n");
    
    // Contiene il nome del FILE, indice dello slave, numero di slaves, PIPE_MASTER, PIPE_SLAVE
    char* argExec[] = {"./slave.out", "0", argv[1], PIPE_MASTER, PIPE_SLAVE, NULL};
    

    // Generating slaves
    for(int i = 1; i < slavesNr; i++){
        sprintf(tmpArg, "%d", i);
        argExec[1] = tmpArg;                // Carica al posto di '0' l' indice dello slave
        if(!fork()){
            execv(argExec[0], argExec);
        }
    }

    //Open the master pipes in write
    for (int i = 0; i < slavesNr; i++){
        // clearOpen pipe in write
        fifoMasterFD[i] = open(fifoMaster[i],O_WRONLY);
    }
    printf("All pipes opened\n");
    fflush(stdout);
    sleep(1);

    ///////////////////////////////////////////////// INPUT MANAGEMENT //////////////////////////////////////////////////////////

    printf("\nNext command: ");
    fflush(stdout);

    char in[MAX_BUF];
    int leftToRight = 0;

    int r = read(STDIN_FILENO, in, MAX_BUF - 1);
    in[r] = '\0';

    printf("%s\n", in);
    int begin = 0;
    int find = 0;
    int sender, receiver;
    char senderString[1200], receiverString[1200], msg[1200];

    for( int i = 0; i < strlen(in); i++ ){
        
        if(in[i] == '>' && find == 0){

            strncpy(senderString, in, (i-begin));
            senderString[i-begin] = '\0';
            begin = i+1;
            leftToRight = TRUE;

        } else if(in[i] == '<' && find == 0){
            
            strncpy(receiverString, in, (i-begin));
            receiverString[i-begin] = '\0';
            begin = i+1;
            leftToRight = FALSE;

        } else if(in[i] == ' ' && find == 0){

            if(leftToRight){
                strncpy(receiverString, in+begin, (i-begin));
                receiverString[i-begin] = '\0';
                begin = i+1;
                find = 1;
            }else if(!leftToRight){
                strncpy(senderString, in+begin, (i-begin));
                senderString[i-begin] = '\0';
                begin = i+1;
                find = 1;
                
            }
        }

    }
    
    strncpy(msg, in+begin, strlen(in) - begin);
    msg[strlen(in)-begin] = '\0';
    sender = atoi(senderString);
    receiver = atoi(receiverString);

    printf("Sender %d\n", sender);
    printf("Receiver %d\n", receiver);
    printf("Message %s\n", msg);


    strcat(receiverString, " ");
    receiverString[strlen(receiverString)] = '\0';
    strcat(receiverString, msg);
    receiverString[strlen(receiverString)-1] = '\0'; // (-1) FIX \n

    printf("Receiver String %s\n", receiverString);

    write(fifoMasterFD[sender], receiverString, strlen(receiverString) + 1);

    while(TRUE);

    return 0;

}