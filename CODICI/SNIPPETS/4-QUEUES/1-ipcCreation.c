#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <fcntl.h>

void main(){
    remove("/tmp/unique");                                              // Remove file
    key_t queue1Key = ftok("/tmp/unique", 1);                           // Get unique key -> FAIL
    creat("/tmp/unique", 0777);                                         // Create file
    queue1Key = ftok("/tmp/unique", 1);                                 // Get unique key -> OK
    int queueId = msgget(queue1Key, 0777 | IPC_CREAT);                  // Create queue -> OK
    queueId = msgget(queue1Key, 0777);                                  // Get queue -> OK
    msgctl(queue1Key, IPC_RMID, NULL);                                  // Remove not existing queue
    msgctl(queueId, IPC_RMID, NULL);                                    // Remove queue -> OK
    queueId = msgget(queue1Key, 0777);                                  // Get non existing queue -> OK
    queueId = msgget(queue1Key, 0777 | IPC_CREAT);                      // Create queue -> OK
    queueId = msgget(queue1Key, 0777 | IPC_CREAT);                      // Get queue -> OK
    queueId = msgget(queue1Key, 0777 | IPC_CREAT | IPC_EXCL);           // Create already existing queue -> fail
}