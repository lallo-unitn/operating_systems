#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>

#define MAX_BUF 100
#define MAX_SLAVES 10

#define PIPE_SLAVE "/tmp/fifoSlave"
#define PIPE_MASTER "/tmp/fifoMaster"

#define TRUE 1 
#define FALSE 0

char fifoMaster[100];
char fifoMasterFD;

// FIFO between slaves
char fifoSlaves[MAX_SLAVES][100];

// FDs of FIFO between slaves
char fifoSlavesFD[MAX_SLAVES];

int slaveIndex;
int canRead = 1;

//Thread for incoming messages
void* slaveCommunication(void * param);

// Method to receive the messages from the master
void  messageReceiveHandler();

int main(int argc, char** argv){    

    // Contiene il nome del FILE, indice dello slave, numero di slaves, PIPE_MASTER, PIPE_SLAVE
    // char* argExec[] = {"./slave.out", "0", argv[1], PIPE_MASTER, PIPE_SLAVE, NULL};
    
    slaveIndex = atoi(argv[1]);
    int slavesNr = atoi(argv[2]);

    printf("___________OPENING READ FIFO SLAVE -> MASTER___________\n");

    strcpy(fifoMaster, PIPE_MASTER);
    strcat(fifoMaster, argv[1]);
    fifoMasterFD = open(fifoMaster, O_RDONLY);
    

    printf("Slave nr [%d] opening [READ] in FIFO [%s] for master\n", slaveIndex, fifoMaster);

    printf("___________OPENING RDWR FIFO SLAVE -> SLAVES___________\n");

    char tmpArg[10];

    pthread_t t_ids[MAX_SLAVES]; //Arrays of threads
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

    for (int i = 1; i <= slavesNr; i++){
        //Craft the various pipe names
        if(i == slaveIndex){
            continue;
        }else if(i<slaveIndex){
            sprintf(tmpArg, "%d%d%d", i, "-", slaveIndex); //2 i= 1  fifo12
        }else{
            sprintf(tmpArg, "%d%d%d", slaveIndex, "-", i); // 23
        }
        strcpy(fifoSlaves[i],argv[4]);
        strcat(fifoSlaves[i],tmpArg);
        //printf("[%d]Using %s\n",slaveIndex,fifoSlave[i-1]);

        //Open fifo in read write mode for slave communication
        fifoSlavesFD[i] = open(fifoSlaves[i], O_RDWR | O_NONBLOCK);

        //Create a thread for each pipe, on which the programm will listen for incoming messages
        pthread_create(t_ids+i, &attr, slaveCommunication, (void *)&fifoSlavesFD[i]);
    
    }

    messageReceiveHandler();

    return 0;
}

// Thread looping while looking for messages in FIFO (void* param;)
void* slaveCommunication(void * param){     
    int fd = *(int*)param;
    char buf[MAX_BUF];
    printf("%s[%d]Thread for watching FD %d%s\n", slaveIndex, fd);

    // Keep on receiving messages
    while(1){
        if(canRead){
            int r = read(fd, buf, MAX_BUF-1);
            if(r > 0){
                buf[r] = 0;
                printf("[%d]Received '%s'\n",slaveIndex , buf);
            }
        }
    }
}

// Method for receiving messages from the master
void  messageReceiveHandler(){
    // Open master pipe
    char buf[MAX_BUF+10], dest[10], msg[MAX_BUF];
    int r, ind, target;

    // Keep on receiving messages (until SIGTERM)
    while(1){
        r = read(fifoMasterFD, &buf, MAX_BUF-1);
        if(r == 0){
            printf("[%d] Nothing to read\n", slaveIndex);
            continue;
        }
        buf[r] = 0;
        ind = 0;
        // Extract target process
        for(int i = 0; i < r; i++){
            if(buf[i] == ' '){
                ind = i;
                break;
            }
        }
        // Store in 'dest' the target node
        strncpy(dest, buf, ind);
        target = atoi(dest);

        // Check if target is valid
        if(target <= 0 || target > MAX_SLAVES){
            printf("[%d]Wrong target '%s'\n", slaveIndex, buf);
            continue;
        }
        strcpy(msg, buf+ind+1);
        // Set canRead to false while sending message on FIFO -O_RDWR-
        canRead = 0;

        // Send message
        write(fifoSlavesFD[target], msg, strlen(msg));
        printf("[%d]Sent '%s' to %d\n", slaveIndex, msg, target);
        sleep(1);
        canRead = 1;
    }
}