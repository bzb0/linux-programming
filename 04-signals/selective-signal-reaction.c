/*****************************************************************************
 * This example shows selective signal handling sent from the pause function.
 ****************************************************************************/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// #define IGNORE_OTHER_SIGNALS

void custom_handler1(int);

void custom_handler2(int);

int flag = 0;

void main() {
    int pid;

    signal(SIGUSR1, custom_handler1);
    signal(SIGUSR2, custom_handler2);

#ifdef IGNORE_OTHER_SIGNALS
            printf("The signal SIGUSR2 sent to the CHILD process will be ignored from the PARENT\n");
            printf("No flag=1 in user2_handler, until (!flag) I'll pause(). Press <Ctrl+C> to end\n");
#endif

    if ((pid = fork()) == -1) {
        printf("Can't spanw a child process.\n");
        exit(-1);
    } else if (pid != 0) {
        // PARENT process
#ifdef IGNORE_OTHER_SIGNALS
            while(!flag)
#else
        if (!flag)
#endif
            pause();

        printf("[PARENT] Received signal from the CHILD process.\n");
        wait(NULL);
        exit(0);
    } else {
        // CHILD process
        printf("[CHILD] Sending SIGUSR2 signal to the PARENT process.\n");
        kill(getppid(), SIGUSR2);
        exit(0);
    }
}

void custom_handler1(int sig) {
    flag = 1;
    signal(SIGUSR1, custom_handler1);
}

void custom_handler2(int sig) {
    // No change of the flag variable, this handler has other purpose
    signal(SIGUSR2, custom_handler2);
}
