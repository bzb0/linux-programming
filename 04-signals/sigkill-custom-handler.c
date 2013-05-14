/**************************************************************************************
 * This example shows that for the SIGKILL signal a custom handler can't be specified.
 *************************************************************************************/

#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void kill_handler();

void main() {
    signal(SIGKILL, kill_handler);
    printf("Performing KILL(9)...\n");
    printf("Press <ENTER>\n");
    getchar();
    kill(getpid(), SIGKILL);
    printf("We'll still be killed. The custom signal handler won't save us.\n");
}

void kill_handler() {
    printf("The custom signal handler can't help us.\n");
}
