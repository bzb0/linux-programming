#include <stdio.h>
#include <stdlib.h>

// this external variable is always avaiable in Linux
extern char **environ;

void view_environment() {
    for (int i = 0; environ[i] != (char *) 0; i++) {
        printf("%s\n", environ[i]);
    }
}

void print_env_variable(const char *env_var) {
    char *ptr;
    if( (ptr = getenv(env_var)) == (char *)0) {
        printf("%s is not defined\n", env_var);
    } else {
        printf("%s\n", ptr);
    }
}

void main() {
    view_environment();
    print_env_variable("PATH");
}
