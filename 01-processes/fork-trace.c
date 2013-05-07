/****************************************************************************
 * This example creates a child process and uses the GNU C compiler
 * feature for retrieving the address of a label, which enables us to
 * follow the changes of the program counter as the parent and child
 * processes are running. The parent and the child process are synchronized.
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int var_label0 = 1;
int var_label1 = 2;
int var_label2 = 3;
int var_label3 = 4;
int var_label4 = 5;
int var_label5 = 6;
int count = 0;

#ifdef __GNUC__
#define OFFSET(label) (&&label)
#else
#define OFFSET(label) (var##label)
#endif

#define PRINT_TRACE(label) printf("\nPID=%d, offset = %lu\n", getpid(), (unsigned long)(OFFSET(label))); count++;

int pid;
int status;

int main() {
label0: PRINT_TRACE(label0);
label1: PRINT_TRACE(label1);

    pid = fork();

label2: PRINT_TRACE(label2);

    if (pid != 0) {
    label3: PRINT_TRACE(label3);
        wait(&status);
        exit(0);
    } else {
        count = 0;
    label4: PRINT_TRACE(label4);
    }

label5: PRINT_TRACE(label5);
    printf("Value of the variable 'count' e: %d\n", count);

    exit(0);
}
