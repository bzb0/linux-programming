/*******************************************************************************
 * The following programs demonstrates a round-robin structure for executing
 * processes. The parent process creates N child process. The processes are
 * synchonized via the system signal kill(). First the parent process spawns
 * a child process, which waits for a singal from the parent. The child process
 * sends a signal to the parent, which in turn sends a signal to the second
 * child process. This is repeated until all child processes finish running.
 ******************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define CHILD_SLEEP_TIME 1
#define TIME_SLICE_NO 3
#define SLEEP for(s_var=0;s_var<500_000;s_var++)

void user_signal1_handler(int);

long s_var;
int start = 0, active, pid;
unsigned int how_many_childs;
int *pid_array;

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("Usage: round-robin <NUM_CHILD_PROCESSES>\n");
        return -1;
    }
    how_many_childs = atoi(argv[1]);
    if (how_many_childs <= 0) return -1;

    if ((pid_array = malloc(how_many_childs * sizeof(int))) == NULL) {
        return -1;
    }
    setvbuf(stdout, NULL, _IONBF, BUFSIZ);

    signal(SIGUSR1, user_signal1_handler);
    system("clear");

    printf("I'm starting to spawn processes\n");

    for (unsigned int count = 0; count < how_many_childs; count++) {
        printf(".");
        switch (pid = fork()) {
            case -1:
                how_many_childs = count;
                printf("Can't create CHILD process.\n");
                printf("There are %d CHILD processes.\n", how_many_childs);
                break;
            case 0:
                while (1) {
                    while (!start) {
                        pause(); // wait for the signal
                    }

                    for (int i = 0; i < TIME_SLICE_NO; i++) {
                        printf("CHILD No=%d\n", count);
                        sleep(CHILD_SLEEP_TIME);
                    }
                    start = 0; // again not active
                    kill(getppid(), SIGUSR1);
                }
                break;
            default:
                printf("\b+");
                pid_array[count] = pid;
        }
    }

    printf("\n");

    for (unsigned int count = 0; count < how_many_childs; count++) {
        printf("(%d) PID=%d\n", count, pid_array[count]);
    }

    printf("\nNumber of CHILD processes is %d\n", how_many_childs);
    printf("\nPress <ENTER>...\n");
    getchar();

    active = 0;
    unsigned int hmc = how_many_childs;
    while (hmc--) {
        kill(pid_array[active], SIGUSR1); // activate first process
        while (!start) {
            pause(); // wait for the signal from the process
        }
        start = 0; // not active, prepare for waiting
        active = (active + 1) % how_many_childs; // pick the next active process
    }

    for (unsigned int count = 0; count < how_many_childs; count++) {
        kill(pid_array[count], SIGKILL); // in the end kill the CHILD processes
    }

    exit(0);
}

void user_signal1_handler(int status) {
    start = 1;
    signal(SIGUSR1, user_signal1_handler);
}
