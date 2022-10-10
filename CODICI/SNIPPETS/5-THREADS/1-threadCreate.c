#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

/*
    int pthread_create(
        pthread_t *__restrict__ __newthread,                    ===         Thread ID
        const pthread_attr_t *__restrict__ __attr,              ===         Attributes
        void *(*__start_routine)(void *),                       ===         Function to be executed
        void *__restrict__ __arg                                ===         Parameters of passed function
    );
*/

void *my_fun(void *param){
    printf("This is a thread that received %d\n", *(int *) param);
    return (void*)3;
}

void main(){
    pthread_t t_id;
    int arg = 10;
    pthread_create(&t_id, NULL, my_fun, (void *)&arg);
    printf("Execute thread with id %ld\n", t_id);
    sleep(3);
}