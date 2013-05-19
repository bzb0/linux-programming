/*******************************************************************************
 * An elementary example for using a pipe in Linux. In this program the process
 * defines a pipe and sends a message over the pipe to itself.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void main() {
    int n; // number of read/written bytes
    int pipefd[2]; // pipefd[0] = READ_FD, pipefd[1] = WRITE_FD
    char buffer[100];
    char *bufferP = "Test string...";

    if(pipe(pipefd) < 0) {
        perror("pipe");
        exit(-1);
    }

    printf("READ_FD = %d, WRITE_FD = %d\n", pipefd[0], pipefd[1]);

    printf("Writing in the pipe: %s\n", bufferP);
    if(write(pipefd[1], bufferP, n = strlen(bufferP) +1) != n) {
        printf("Error occurred while writing in the pipe\n");
        exit(-1);
    }

    printf("Reading from the pipe...\n");
    if((n = read(pipefd[0], buffer, sizeof(buffer))) <= 0) {
        printf("Error occurred while reading from the pipe\n");
        exit(-1);
    }
    printf("Read from the pipe: %s\n", buffer);

    close(pipefd[0]);
    close(pipefd[1]);
    exit(0);
}
