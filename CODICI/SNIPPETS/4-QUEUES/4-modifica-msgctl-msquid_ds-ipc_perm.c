#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/*
    int msgctl(int msqid, int cmd, struct msqid_ds *buf);

    > msqid     ===     queue id
    
    > cmd       ===     command used to modify the queue [FLAGS]
                >>>     IPC_STAT    =>  get info from kernel
                >>>     IPC_SET     =>  set some parameters depending from 'buf'
                >>>     IPC_RMID    =>  remove queue instantly
                >>>     IPC_INFO    =>  get general info about system queues limitations
                >>>     MSG_INFO    =>  like IPC_INFO but with info about messages
                >>>     MSG_STAT    =>  like IPC_STAT but with info about messages
    
    > buf       ===     buffer we will fill with info about the queue (last writing time, last reading time, number of messages, ...)
*/
/*
// Defining ipc_perm structure
struct ipc_perm{
    key_t __key;            // Key supplied by msgget(2)
    uid_t uid;              // Effective UID owner
    gid_t gid;              // Effective GID owner
    uid_t cuid;             // Effective UID creator
    gid_t cgid;             // Effective GID creator
    unsigned shot mode;     // Permissions
    unsigned shot __seq;    // Sequence number
}

// Defining msqid_ds structure
struct msqid_ds{
    struct ipc_perm msg_perm;   // Ownership and permissions
    time_t msg_stime;           // Time of last msgsnd(2)
    time_t msg_rtime;           // Time of last msgrcv(2)
    time_t msg_ctime;           // Time of creation OR last modification by msgctl
    unsigned long   msg_cbytes; // # of bytes in the queue
    msgqnum_t       msg_qnum;   // # of messages in the queue
    msglen_t        msg_qbytes; // Maximum # of bytes in the queue
    pid_t           msg_lspid;  // PID of last msgsnd(2)
    pid_t           msg_lrpid;  // PID of last msgrcv(2)
};
*/

// modifica.c

struct msg_buffer{
    long mtype;
    char mtext[100];
}   msgpSND, msgpRCV;

void main(){
    struct msqid_ds mod;
    msgpSND.mtype = 1;
    strcpy(msgpSND.mtext, "This is a message from sender");     // INIT message
    key_t queue1Key = ftok("/temp/unique", 1);
    int queueId = msgget(queue1Key, 0777 | IPC_CREAT);          // Create queue
    msgctl(queueId, IPC_RMID, NULL);                            // Remove queue if exists
    queueId = msgget(queue1Key, 0777 |IPC_CREAT);               // Create queue
    
    msgsnd(queueId, &msgpSND, sizeof(msgpSND.mtext), 0);        // Send message
    msgsnd(queueId, &msgpSND, sizeof(msgpSND.mtext), 0);        // Send message
    msgsnd(queueId, &msgpSND, sizeof(msgpSND.mtext), 0);        // Send message

    msgctl(queueId, IPC_STAT, &mod);                            // Modify queue
    printf("Msg in queue: %ld\nCurrent max bytes in queue: %ld\n\n", 
        mod.msg_qnum, mod.msg_qbytes);

    mod.msg_qbytes = 200;                                        // Change 'buf' in order to modify queue bytes
    msgctl(queueId, IPC_SET, &mod);                             // Apply modifications
    printf("Msg in queue: %ld --> same number\nCurrent max bytes in queue: %ld\n\n", 
        mod.msg_qnum, mod.msg_qbytes);
    
    if(fork() != 0){                                            // parent keeps on writing on the queue
        printf("[SND] Sending third message with a full queue...\n");
        msgsnd(queueId, &msgpSND, sizeof(msgpSND.mtext), 0);                    // Send message
        printf("[SND] Message sent\n");
        printf("[SND] Sending fourth message with IPC_NOWAIT...\n");
        if(msgsnd(queueId, &msgpSND, sizeof(msgpSND.mtext), IPC_NOWAIT) == -1){ // Send message
            perror("Queue is full --> Error");
        }
    } else{                                                     // Child keeps reading the queue every 3 seconds
        sleep(3);
        msgrcv(queueId, &msgpRCV, sizeof(msgpRCV.mtext), 1, 0);
        printf("[RCV] Received message #1 with following text: '%s'\n", msgpRCV.mtext);
        sleep(3);
        msgrcv(queueId, &msgpRCV, sizeof(msgpRCV.mtext), 1, 0);
        printf("[RCV] Received message #2 with following text: '%s'\n", msgpRCV.mtext);
        sleep(3);
        msgrcv(queueId, &msgpRCV, sizeof(msgpRCV.mtext), 1, 0);
        printf("[RCV] Received message #3 with following text: '%s'\n", msgpRCV.mtext);
        sleep(3);
        msgrcv(queueId, &msgpRCV, sizeof(msgpRCV.mtext), 1, 0);
        printf("[RCV] Received message #4 with following text: '%s'\n", msgpRCV.mtext);
        sleep(3);
        if(msgrcv(queueId, &msgpRCV, sizeof(msgpRCV.mtext), 1, IPC_NOWAIT) == -1){
            perror("Reader Error");
        } else{
            printf("[RCV] Received message #5 with following text: '%s'\n", msgpRCV.mtext);
        }
    }
    while(wait(NULL) > 0);
}