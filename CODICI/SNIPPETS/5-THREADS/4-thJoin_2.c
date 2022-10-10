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
    printf("This is a thread that received %d\n", *(int *)param);
    return (void*)3;
}
void main(){
    pthread_t t_id;
    int arg=10;
    int retval;
    pthread_create(&t_id, NULL, my_fun, (void *)&arg);
    printf("Executed thread with id %ld\n",t_id);
    sleep(3);
    pthread_join(t_id, (void **)&retval); 
    printf("retval=%d\n", retval);
}