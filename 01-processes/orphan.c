/******************************************************************************
 * Example of an orphan process. The parent process ends faster than the child
 * process and the child process gets a new parent, namely the init process.
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void main() {
    printf("The PARENT process has PID = %d, PPID = %d \n", getpid(), getppid());

    if (fork() != 0) {
        printf("The PARENT process ends. The shell also finished running the parent process, \n");
        // "Process-ot roditel ZAVRSUVA. Shell-ot isto taka zavrsil so izveduvanjeto na roditelskiot process, taka da vo slednite 5 sec povikajte nekoja komanda od shell prompt-ot (na primer 'ls -l').\n");
        exit(0);
    } else {
        sleep(5);
        printf("This is the CHILD process which got a new PARENT process (the init process).\n");
        printf("PID = %d, PPID = %d \n", getpid(), getppid());
    }

    printf("This code segment will only be executed by the CHILD process. Press <ENTER> for the promt to be visible again.\n");

    exit(0);
}
