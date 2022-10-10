#include <signal.h>

int main(){
    struct sigaction sa;
    sigset_t mod, old;
    sigfillset(&mod);         // Add all signal to the blocked list
    sigemptyset(&mod);          // Remove all signals from blocked list
    sigaddset(&mod, SIGALRM);   // Add SIGALRM to blocked list
    sigismember(&mod, SIGALRM); // Return true if SIGALRM is in mod
    sigdelset(&mod, SIGALRM);   // Remove SIGALRM from mod

    //Update the current mask with the signals in mod
    sigprocmask(SIG_BLOCK, &mod, &old);
    sa.sa_mask = mod;
    return 0;
}