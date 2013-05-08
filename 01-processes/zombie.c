/******************************************************************
 * Demonstration of a zombie process. The process calls exit()
 * but its parent process doesn't finish and stays in the memory.
 *****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void main() {
    if (fork() != 0) {
        printf("The PARENT process never finishes.\n");
        while (1);
    } else {
        printf(
            "I'm the CHILD process, but the PARENT process isn't waiting on me by calling wait(), but stays active and I can't get a new parent (init)\n");
        printf("Because of this I'm becoming a ZOMBIE.\n");
    }
    exit(111);
}
