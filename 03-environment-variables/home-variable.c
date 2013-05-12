#include <stdio.h>
#include <stdlib.h>

void main() {
    char *ptr;
    if ((ptr = getenv("HOME")) == (char *) 0) {
        printf("HOME variable not set.\n");
    } else {
        printf("$HOME=%s\n", ptr);
    }
}
