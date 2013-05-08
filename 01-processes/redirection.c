/***********************************************************************
 * Example of redirection of STDOUT to a file using the system cal dup2.
 **********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

void main(int argc, char *argv[]) {
    printf("Usage: redirection <OUTPUT_FILE> <COMMAND>");

    int fd = open(argv[1], O_CREAT | O_TRUNC | O_WRONLY, 0600);
    dup2(fd, 1);
    close(fd);
    execvp(argv[2], &argv[2]);
    perror("main");
}
