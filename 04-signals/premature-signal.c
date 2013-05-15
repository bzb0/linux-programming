/************************************************************************************
 * The prematurely sent signal problem in Linux is demonstrated in this example.
 * Namely, if the signal is sent before the process starts waiting on this signal,
 * then the process will wait until the next signal is sent. This is a typical Linux
 * problem. Here this problem is solved with a custom signal handler, in which
 * a flag variable is set if a signal was already sent.
 ***********************************************************************************/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// #define SIGNAL_BEFORE_PAUSE

int flag = 0;

void custom_handler(int);

// This function should force the scheduler to switch to another process
void do_something_time_consuming() {
    for(long int i=0;i<50000000;i++);
}

void main() {
    int pid;
    signal(SIGUSR1, custom_handler);

    if( (pid = fork()) == -1) {
        printf("Can't spawn a child process.\n");
        exit(-1);
    } else if(pid != 0) {
        // PARENT process
        #ifdef SIGNAL_BEFORE_PAUSE
        do_something_time_consuming();
        #endif

        if(!flag) {
            printf("Pausing execution\n");
            pause();
        } else {
            printf("Signal sent before pause()\n");
        }
        printf("[PARENT] Waiting for the CHILD process to die.\n");
        wait(NULL);
        exit(0);
    } else {
        // CHILD PROCESS
        printf("[CHILD] Sending SIGUSR1 signal to PARENT process.\n");
        kill(getppid(), SIGUSR1);
        exit(0);
    }
}

void custom_handler(int sig) {
    printf("[SIGNAL HANDLER] Received SIGUSR1 signal.\n");
    flag = 1;
    signal(SIGUSR1, custom_handler);
}