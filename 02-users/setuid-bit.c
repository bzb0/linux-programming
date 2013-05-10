/************************************************************************
 * This example demonstrates how to set the setuid permission on a file.
 ***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

struct stat StatBuffer;

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("Usage: set-uid-bit <FILE_NAME>\n");
        return 1;
    }

    printf("stat() helps to fetch the file MODE\n");

    if (stat(argv[1], &StatBuffer) < 0) {
        perror("stat");
        exit(-1);
    }

    StatBuffer.st_mode |= S_ISUID; // new_mode = old_mode + setuid bit

    if (chmod(argv[1], StatBuffer.st_mode) < 0) {
        perror("chmod");
        exit(-1);
    }
}
