#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

/* 
    ssize_t msgrcv(int msquid, void *msgp, size_t msgsz, long msgtyp, int msgflg);
    
    > msqid     ===     queue
    > msgp      ===     struct message
    > msgsz     ===     message text size
    > msgtyp    >>>     msgtyp == 0     =>  get first message in the queue
                >>>     msgtyp > 0      =>  get first message of type 'msgtyp' OR get first message 
                                            with message type != msgtyp if MSG_EXCEPT
                >>>     msgtyp < 0      =>  first message with type T s.t. [T <= |msgtype|]
    > msgflg    ===     flags
                >>>     MSG_NOERROR     =>  truncate message if bigger then size; if not specified, the syscall 
                                            will fail for messages bigger then size
                >>>     MSG_EXCEPT      =>  see above
                >>>     IPC_NOWAIT      =>  syscall will fail if no messages in the queue exist; if not specified, 
                                            the syscall will hang
*/

struct msg_buffer{
    long mtype;
    char mtext[100];
} msgp1, msgp2;

void main(){
    msgp1.mtype = 20;
    strcpy(msgp1.mtext, "This is a message");
    key_t queue1Key = ftok("/tmp/unique", 1);
    int queueId = msgget(queue1Key, 0777 | IPC_CREAT | IPC_EXCL);
    int esito = msgsnd(queueId, &msgp1, sizeof(msgp1.mtext), 0);
    esito = msgrcv(queueId, &msgp2, sizeof(msgp2.mtext), 20, 0);
}