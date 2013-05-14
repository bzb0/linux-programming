/**************************************************************************************
 * This programs shows an implementation of a custom alarm handler. Instead of an exit
 * the program executes the custom alarm_handler function. The program demonstrates
 * the usage of the function raise(), which is identical to kill(getpid(), signal).
 *************************************************************************************/

#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#define GENERATE_DEFAULT

void alarm_handler();

void main() {
    signal(SIGALRM, alarm_handler);
    alarm(5);
    while (1);
}

void alarm_handler() {
    printf("Alarm received\n");
#ifdef GENERATE_DEFAULT
    raise(SIGKILL);
#endif
}
