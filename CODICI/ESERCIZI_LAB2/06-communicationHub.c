#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>

#define WRITE 0
#define READ 1
#define MAX_ARG 10
#define MAX_BUF 50
#define MAX_SLAVES 20
#define MAX_NAME 20

char stringIndex[5];
int isChild = 0;
int slaveIndex = 0;


int main(int argc, char **argv)
{

    int fdWriteMaster[MAX_SLAVES];
    int fdSlaveToSlave[MAX_SLAVES];
    int fdReadFromMaster;
    char fifoMasterName[MAX_NAME] = "/tmp/fifo";

    char in[MAX_ARG];
    char buf[MAX_BUF];
    char message[MAX_BUF];

    int directIndex;
    int spaceIndex;
    int sender;
    int receiver;
    int leftToRight;

    char senderIn[MAX_BUF];
    char receiverIn[MAX_BUF];

    char senderString[1200], receiverString[1200], msg[1200];

    printf("MASTER PID: [%d]\n", getpid());

    printf("___________CREATING FIFO MASTER -> SLAVES___________\n");

    int slavesNr = atoi(argv[1]);
    for (int i = 1; i <= slavesNr; i++)
    {
        sprintf(stringIndex, "%d", i);
        mkfifo(strcat(fifoMasterName, stringIndex), S_IRUSR | S_IWUSR); // Creating FIFO with index i
        perror("Created?");
        fdWriteMaster[i] = open(fifoMasterName, O_RDWR); // Open fd Master -> Slave[i]
        printf("%s\n", fifoMasterName);
        strcpy(fifoMasterName, "/tmp/fifo");
    }

    printf("\n");

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    printf("___________CREATING FIFO SLAVES -> SLAVES___________\n");

    for (int i = 1; i <= slavesNr; i++)
    {
        for (int j = i + 1; j <= slavesNr; j++)
        {
            sprintf(stringIndex, "%d", i);
            strcat(fifoMasterName, stringIndex);

            strcat(fifoMasterName, "-");
            sprintf(stringIndex, "%d", j);
            strcat(fifoMasterName, stringIndex);

            mkfifo(fifoMasterName, S_IRUSR | S_IWUSR); // Creating FIFO with index i
            perror("Created?");
            printf("%s\n", fifoMasterName);
            strcpy(fifoMasterName, "/tmp/fifo");
        }
    }

    printf("\n");

    printf("___________OPENING READ SLAVES -> MASTER___________\n");

    for (int i = 1; i <= slavesNr; i++)
    {
        isChild = !fork();
        if (isChild)
        {
            slaveIndex = i;
            sprintf(stringIndex, "%d", i);
            strcat(fifoMasterName, stringIndex);
            printf("Slave nr %d opening [READ] from master in FIFO [%s]\n", i, fifoMasterName);
            fflush(stdout);
            fdReadFromMaster = open(fifoMasterName, O_RDONLY);  // fdReadMS[i] file descriptor for reading master from
            fcntl(fdReadFromMaster, F_SETFL, O_NONBLOCK);       // read() will not hang
            strcpy(fifoMasterName, "/tmp/fifo");
            break; // Only master will loop
        }
    }

    sleep(2);

    if (!isChild)
    {
        printf("\n");
        printf("PRINTER PID: [%d]\n", getpid());
        printf("___________OPENING READ & WRITE SLAVE -> SLAVE___________\n");
        fflush(stdout);
    }

    sleep(2);

    if (isChild)
    {
        for (int i = slaveIndex + 1; i <= slavesNr; i++)
        {
            sprintf(stringIndex, "%d", slaveIndex);
            strcat(fifoMasterName, stringIndex);

            strcat(fifoMasterName, "-");
            sprintf(stringIndex, "%d", i);
            strcat(fifoMasterName, stringIndex);
            printf("Slave nr [%d] opening [READ] & [WRITE] for slave [nr] %d in FIFO [%s]\n", slaveIndex, i, fifoMasterName);
            fflush(stdout);

            fdSlaveToSlave[i] = open(fifoMasterName, O_RDWR);
            fcntl(fdSlaveToSlave[i], F_SETFL, O_NONBLOCK);
            strcpy(fifoMasterName, "/tmp/fifo");
        }

        sleep(2);

        for (int i = slaveIndex - 1; i > 0; i--)
        {
            sprintf(stringIndex, "%d", i);
            strcat(fifoMasterName, stringIndex);

            strcat(fifoMasterName, "-");
            sprintf(stringIndex, "%d", slaveIndex);
            strcat(fifoMasterName, stringIndex);
            printf("Slave nr [%d] opening [READ] & [WRITE] for slave [nr] %d in FIFO [%s]\n", slaveIndex, i, fifoMasterName);
            fflush(stdout);

            fdSlaveToSlave[i] = open(fifoMasterName, O_RDWR);
            fcntl(fdSlaveToSlave[i], F_SETFL, O_NONBLOCK);
            strcpy(fifoMasterName, "/tmp/fifo");
        }
        while(true){
            int begin = 0;
            int find = 0;
            int endMasterLoop = 0;
            
            if(read(fdReadFromMaster, senderIn, sizeof(senderIn)) > 0){
                for( int i = 0; i < strlen(senderIn); i++ ){
                    if(senderIn[i] == ' ' && find == 0){
                        strncpy(receiverString, senderIn+begin, (i-begin));
                        receiverString[i-begin] = '\0';
                        begin = i+1;
                        find = 1;
                        strncpy(msg, senderIn+begin, strlen(senderIn) - begin);
                        msg[strlen(senderIn)-begin] = '\0';
                        receiver = atoi(receiverString);

                        char recAndMsg[MAX_BUF];
                        strcpy(recAndMsg, receiverString);
                        recAndMsg[strlen(recAndMsg)] = '\0';
                        strcat(recAndMsg, " ");
                        recAndMsg[strlen(recAndMsg)] = '\0';
                        strcat(recAndMsg, msg);
                        recAndMsg[strlen(recAndMsg)] = '\0';
                        
                        printf("Slave [%d] sending message [%s] to slave [%d]\n\n", slaveIndex, recAndMsg, receiver);
                        endMasterLoop = 1;

                        for(int j = 1; j <= slavesNr; j++){
                            if(j != slaveIndex){
                                printf("Slave [%d] sending message [%s] to slave [%d]\n", slaveIndex, recAndMsg, j);
                                write(fdSlaveToSlave[j], msg, strlen(msg) + 1);
                            } 
                        }
                    }
                }
            }
            if(endMasterLoop){
                printf("DEBUG 1\n");
                for(int j = 1; j <= slavesNr; j++){
                    printf("DEBUG 2\n");
                    if(j != slaveIndex){
                        if(read(fdSlaveToSlave[j], senderIn, sizeof(senderIn)) > 0){
                            printf("DEBUG 3\n");
                            for( int i = 0; i < strlen(senderIn); i++ ){
                                printf("DEBUG 4\n");
                                if(senderIn[i] == ' ' && find == 0){
                                    strncpy(receiverString, senderIn+begin, (i-begin));
                                    receiverString[i-begin] = '\0';
                                    receiver = atoi(receiverString);
                                    printf("Receiver seen from slave: [%d]/n", receiver);
                                    begin = i+1;
                                    find = 1;
                                    if(receiver == slaveIndex){
                                        strncpy(msg, senderIn+begin, strlen(senderIn) - begin);
                                        msg[strlen(senderIn)-begin] = '\0';
                                        printf("Slave [%d] received message [%s]\n", slaveIndex, msg);
                                    }
                                }
                            }
                        }
                    }
                }
                endMasterLoop = 0;
            }
                

            /*for(int j = 0; j < MAX_SLAVES; j++){
                
                if(read(fdSlaveToSlave[j], senderIn, sizeof(senderIn)) >= 0)){
                    for( int i = 0; i < strlen(senderIn); i++ ){
                        if(senderIn[i] == ' ' && find == 0){
                            if(slaveIndex == senderIn[0]){
                                printf("Slave [%d] received message [%s]", slaveIndex, msg);
                            }
                        }
                    }
                }
            }*/
        }   
    }

    sleep(3);

    printf("\nNext command: ");
    fflush(stdout);

    int r = read(STDIN_FILENO, in, MAX_BUF - 1);
    in[r] = '\0';

    printf("%s\n", in);
    int begin = 0;
    int find = 0;
    for( int i = 0; i < strlen(in); i++ ){
        
        if(in[i] == '>' && find == 0){
            strncpy(senderString, in, (i-begin));
            senderString[i-begin] = '\0';
            begin = i+1;
            leftToRight = true;
        } else if(in[i] == '<' && find == 0){
            strncpy(receiverString, in, (i-begin));
            receiverString[i-begin] = '\0';
            begin = i+1;
            leftToRight = false;
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

    write(fdWriteMaster[sender], receiverString, strlen(receiverString) + 1);

    while(true);
    
    return 0;
}