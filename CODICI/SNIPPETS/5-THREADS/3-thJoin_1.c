#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

/*
    Starting a new thread within a thread, we can wait the new thread termination using the following function:
    
    int pthread_join(pthread_t thread, void **retval);
    
        > thread    ===     THREAD ID of the thread we are going to wait
        > retval    ===     if the terminated thread returns a non-zero value, this will be saved in the variable pointed by 'retval'
                    >>>     [ PTHREAD_CANCELED ]        =>      if the thread has been canceled
                    
    We can wait a thread if and only if it is 'joinable'. 
    A thread can be waited by one thread at most.
*/

void *my_fun(void *param){
    printf("Thread %ld started\n", *(pthread_t *)param);
    sleep(3);
    char *str = "Returned string";
    pthread_exit(str);
}

void main(){
    pthread_t t_id;
    void *retFromThread;
    pthread_create(&t_id, NULL, my_fun, (void*)&t_id);
    pthread_join(t_id, &retFromThread);
    printf("Thread %ld returned '%s'\n", t_id, (char *)retFromThread);
}