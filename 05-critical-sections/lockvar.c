/*********************************************************************************
 * This example demonstrates a definition of a critical section. The critical
 * section is defined with the lock variable lock_var. The example also shows
 * the fragility of this method, namely that two processes can enter the critical
 * section. For instance, the first process enters the critical section as the lock
 * variable is set to 0, but while it sets the lock variable to 1 the other
 * process also sees that the lock variable is 0 and enters the critical section.
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>

#define CREATE_LOCK lock = open("lock.mem", O_RDWR | O_CREAT | O_TRUNC, 0666); if(lock == -1) { perror("Can't create lock file"); exit(-1); }
#define OPEN_LOCK lock = open("lock.mem", O_RDWR, 0666); if(lock == -1) { perror("Can't open lock file"); exit(-1); }
#define CREATE_RESOURCE resource = open("resource.mem", O_RDWR | O_CREAT | O_TRUNC, 0666); if(resource == -1) {  perror("Can't create resource file"); exit(-1); }

#define SETLOCKTO(lock_valueP) write(lock, lock_valueP, 1); lseek(lock, 0, L_SET);
#define READLOCK(lock_valueP) read(lock, lock_valueP, 1); lseek(lock, 0, L_SET);

// #define PROCESS1_ERR

const char UNLOCKED = 0;
const char LOCKED = 1;

int lock_var, lock, resource, err, pid1, pid2;

void wait_until_unlocked(char *);

void enter_critical_section(char *);

void main() {
    CREATE_RESOURCE;
    CREATE_LOCK;
    SETLOCKTO(&UNLOCKED);

    // making STDOUT unbuffered
    setvbuf(stdout, NULL, _IONBF, BUFSIZ);

    switch (pid1 = fork()) {
        case -1:
            printf("Can't spawn a child process.\n");
            exit(-1);
        case 0:
            // child process
            OPEN_LOCK;
#ifdef PROCESS1_ERR
                sleep(1);
#endif
            while (1) {
                wait_until_unlocked("1");

                enter_critical_section("1");
            }
            break;
        default:
            // PARENT PROCESS
            switch (pid2 = fork()) {
                case -1:
                    printf("Can't spawn a child process.\n");
                    exit(-2);
                case 0:
                    // CHILD PROCESS
                    OPEN_LOCK;
                    while (1) {
                        wait_until_unlocked("2");

                        enter_critical_section("2");
                    }
                    break;
                default:
                    // PARENT PROCESS
                    close(lock);
                    close(resource);
                    while (1);
                    exit(0); // exit main process
            }
            break;
    }
}

void wait_until_unlocked(char *process_id) {
    do {
        // reading the lock_var
        READLOCK(&lock_var);
        printf("\nWAIT(%s)\n", process_id);
        sleep(2);
    } while (lock_var != UNLOCKED); // read & wait
}

void enter_critical_section(char *process_id) {
    SETLOCKTO(&LOCKED);
    // begin of critical section
    err = flock(resource, LOCK_EX | LOCK_NB);
    if (err == -1) {
        printf("Process %s received an ERROR while trying to lock the file\n", process_id);
        close(lock);
        close(resource);
        printf("Press Ctrl+C to end.\n");
        exit(-1);
    }
    printf("\n--> IN(%s)\n", process_id);
    sleep(2);
    flock(resource, LOCK_UN);
    printf("\nUNLOCKED(%s)\n", process_id);
    // end of critical section
    SETLOCKTO(&UNLOCKED);
}
