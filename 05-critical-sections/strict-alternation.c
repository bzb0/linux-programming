/*************************************************************************
 * This example demonstrates a strict process alternation. The processes
 * execution is alternating, in other words only one process executes at
 * one time. There could be a problem if one of the processes dies during
 * the message exchange. The other process will wait indefinitely.
 ************************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/wait.h>

#define CREATE_LOCK lock = open("turn.mem", O_RDWR | O_CREAT | O_TRUNC, 0666); if(lock == -1) { perror("Can't create lock file"); exit(-1); }
#define OPEN_LOCK lock = open("turn.mem", O_RDWR, 0666); if(lock == -1) { perror("Can't open lock file"); exit(-1); }
#define CREATE_RESOURCE resource = open("resource.mem", O_RDWR | O_CREAT | O_TRUNC, 0666); if(resource == -1) {  perror("Can't create resource file"); exit(-1); }

#define TURN_TO(lock_valueP) \
    if(getpid() == pid1) printf("\nProcess-1 enters critical section and "); \
    if(getpid() == pid2) printf("\nProcess-2 enters critical section and "); \
    printf("TURNs TO process: %d\n", *lock_valueP); \
    write(lock, lock_valueP, 1); \
    lseek(lock, 0, L_SET);
#define READLOCK(lock_valueP) read(lock, lock_valueP, 1); lseek(lock, 0, L_SET);

const char ONE = 1;
const char TWO = 2;

int pid1;
int pid2;
int lock;
int resource;
int lock_var;
int err;
int counter1 = 10;
int counter2 = 10;

void acquire_lock(char *, int *);

void enter_critical_section(char *, int *);

void main() {
    CREATE_RESOURCE;
    CREATE_LOCK;

#ifdef PROCESS2_FIRST
        TURN_TO(&ONE);
#else
    TURN_TO(&ONE);
#endif

    // making STDOUT unbuffered
    setvbuf(stdout, NULL, _IONBF, BUFSIZ);

    switch (pid1 = fork()) {
        case -1:
            printf("Can't spawn child process\n");
            exit(-1);
        case 0:
            // CHILD PROCESS
            pid1 = getpid();
            OPEN_LOCK;
            while (1) {
                READLOCK(&lock_var);
                while (lock_var != ONE) {
                    if (!counter1--) {
                        printf("Process-1 can't enter the critical section, because the other process hasn't released the resource.");
                        close(lock);
                        close(resource);
                        exit(-1);
                    }
                    printf("\nWAIT(1)\n");
                    sleep(2);
                    READLOCK(&lock_var);
                }

                // enter critical section
                err = flock(resource, LOCK_EX | LOCK_NB);
                if (err == -1) {
                    printf("Process-1 received an ERROR while trying to lock the file\n");
                    exit(-1);
                }
                counter1 = 10;
                printf("\n-->IN(1)\n");
                sleep(2);
                flock(resource, LOCK_UN);
                printf("\nUNLOCKED(1)\n");
                // end critical section

                TURN_TO(&TWO);
            }
            break;
        default:
            // PARENT PROCESS
            switch (pid2 = fork()) {
                case -1:
                    printf("Can't spawn child process.\n");
                    exit(-2);
                case 0:
                    // CHILD PROCESS
                    pid2 = getpid();
                    OPEN_LOCK;
                    while (counter2--) {
                        READLOCK(&lock_var);
                        while (lock_var != TWO) {
                            printf("\nWAIT(2)\n");
                            sleep(2);
                            READLOCK(&lock_var);
                        }

                        // enter critical section
                        err = flock(resource, LOCK_EX | LOCK_NB);
                        if (err == -1) {
                            printf("Process-2 received an ERROR while trying to lock the file\n");
                            exit(-1);
                        }
                        printf("\n-->IN(2)\n");
                        sleep(2);
                        flock(resource, LOCK_UN);
                        printf("\nUNLOCKED(2)\n");

                        // end critical section
                        TURN_TO(&ONE);
                    }
                    printf("Process-2 terminates...\n");
                    exit(0);
                default:
                    // PARENT PROCESS
                    close(lock);
                    close(resource);
                    while (wait(NULL) != -1);
                    exit(0); // exit main process
            }
            break;
    }
}
