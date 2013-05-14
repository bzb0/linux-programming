/*******************************************************************
 * This example demonstrates parallel synchonized processing, where
 * the child process sends the SIGALRM (alarm function) to the
 * parent process and simulates a timer.
 ******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void main() {
    int status;
    if (fork()) {
        printf("[PARENT] Waiting for the child to finish execution\n");
        wait(&status);
        printf("[PARENT] Parent process exits, since the child also exited...\n");
        exit(0);
    } else {
        alarm(5);
        printf("[CHILD] Waiting for the alarm\n");
        while (1);
    }
}
