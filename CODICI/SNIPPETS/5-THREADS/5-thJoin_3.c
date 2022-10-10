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
    sleep(2);
}

void *my_fun(void *param){
    if(pthread_join( *(pthread_t *)param, NULL) != 0){
        printf("Error\n");
    }
}

void main(){
    pthread_t t_id1, t_id2;
    pthread_create(&t_id1, NULL, my_fun, NULL);             // Create
    pthread_create(&t_id2, NULL, my_fun, (void *)&t_id1);   // Create
    pthread_join(t_id1, NULL);                              // Wait thread
    sleep(1);
    perror();
}

/*
    Threads are created with attributes specified in the [ pthread_attr_t ] structure,
    This struct can be initialized using the function [ int pthread_attr_init(pthread_attr_t *attr); ]; it sets all 
    the attributes at their default value. When this struct is no longer necessary, we can delete it using the 
    function [ int pthread_attr_destroy(pthread_attr_t *attr); ].

    The value of the attributes must be modified using the following the two following functions:

        (1) [ int pthread_attr_setxxxx(pthread_attr_t *attr, params); ]
        (2) [ int pthread_attr_getxxxx(pthread_attr_t *attr, params); ]

    The attributes are the following:

        > ...detachstate(pthread_attr_t, int detachstate)
            >>> [ PTHREAD_CREATE_DETACHED ]     =>      cannot be waited
            >>> [ PTHREAD_CREATE_JOINABLE ]     =>      default, can be waited

                +++ modified with [ int pthread_detach(pthread_t thread); ]

        > ...sigmask_np(pthread_attr_t *attr, const sigset_t *sigmask);

*/