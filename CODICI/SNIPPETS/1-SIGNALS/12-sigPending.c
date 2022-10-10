#include <signal.h>
#include <unistd.h>
#include <stdio.h>

int main(){
    sigset_t mod, pen;
    sigemptyset(&mod);
    sigaddset(&mod, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mod, NULL);
    kill(getpid(), SIGUSR1);    // Sent but it's blocked...        
    sigpending(&pen);           // ... still pending
    printf("%d\n",(int)sigismember(&pen, SIGUSR1));  // Return 1 if SIGNO is in SET, 0 if not.
    return 0;
}