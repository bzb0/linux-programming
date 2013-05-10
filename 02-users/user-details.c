#include <stdio.h>
#include <unistd.h>

void main() {
    printf("Parent's real UID: %d\n", getuid());
    printf("Parent's real GID: %d\n", getgid());

    printf("Parent's effective UID: %d\n", geteuid());
    printf("Parent's effective GID: %d\n", getegid());
}