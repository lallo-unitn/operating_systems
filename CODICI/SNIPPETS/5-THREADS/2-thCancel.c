#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

/*
    Thread trmination conditions:
        > The thread calls [ nonreturn void pthread_exit(void *retval); ] specifying a return pointer   OR
        > Returns from the associated function specifying a return value                                OR
        > It gets deleted                                                                               OR
        > Another thread calls [ exit() ], terminating all the other threads                            OR
        > The thread executing the [ main() ] function returns, terminating all the others threads

    int pthread_cancel(pthread_t thread);

    This function send a cancellation request to the specified thread. The thread response will be based on its [ state ]
    and its [ type ].
        
        > state     >>>     [ disabled ]        =>  the request will hang until the state become [ enable ]
                    >>>     [ enabled ]         =>  the response depends on [ type ]
        
        > type      >>>     [ deferred ]        =>  set as default; the cancellation will wait until a cancellation point is called
                    >>>     [ asynchronous ]    =>  the cancellation will be immediately executed

        Cancellation points are functions defined in <pthread.h>.  [[ https://man7.org/linux/man-pages/man7/pthreads.7.html  ]]

    Following, the function we use to set [ state ] and [ type ]:

    (1) int pthread_setcancelstate(int state, int *oldstate);

        [ state ] being
            state = PTHREAD _CANCEL_DISABLE     OR
            state = PTHREAD _CANCEL_ENABLE
        
    (2) int pthread_setcanceltype(int type, int *oldtype);

        [ type ] being:
            type = PTHREAD _CANCEL_DEFERRED      OR
            type = PTHREAD _CANCEL_ASYNCHRONOUS
*/

int i = 1;
void *my_fun(void *param){
    if(i--){
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);   // Change state
    }
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);       // Change type
    printf("Thread %ld started\n", *(pthread_t *)param);
    sleep(3);
    printf("Thread %ld finished\n", *(pthread_t *)param);
}

void main(){
    pthread_t t_id1, t_id2;
    pthread_create(&t_id1, NULL, my_fun, (void *)&t_id1);           // Create
    sleep(1);
    pthread_cancel(t_id1);
    printf("[CNL] Cancellation request for thread %ld sent\n", t_id1);
    pthread_create(&t_id2, NULL, my_fun, (void *)&t_id2);           // Create
    sleep(1);
    pthread_cancel(t_id2);
    printf("[CNL] Cancellation request for thread %ld sent\n", t_id2);
    sleep(5);
    printf("Terminating program\n");
}