#include <stdio.h>
#include <unistd.h>

#define PRINT(text) printf("\nPID %3d, PPID = %3d --> " text, getpid(), getppid())

int main() {
    PRINT("The parent process..\n");
    int pid = fork();

    if (pid != 0) {
        PRINT("Code segment which will be executed in the PARENT process.\n");
    } else {
        PRINT("Code segment which will be executed in the CHILD process.\n");
    }

    PRINT("Both processes share this code segment.\n");

    if (pid == 0) {
        PRINT("The PARENT process finished faster than the CHILD process.\n");
        PRINT("Press <ENTER> for the promt to be visible...\n");
    }

    return 0;
}
