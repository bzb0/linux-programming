/************************************************************************************
 * The concept of a sempahore in Linux is demonstrated in the following example. In
 * the example we lock a resource, whereby the lock variable is implemented through
 * semaphores. The following system calls allow us to work with semaphores:
 * - semget(): creates a ns semaphore
 * - semop(): performs operations on a semaphore
 * - semctl(): retrieves information about a semaphore; removes a semaphore
 ***********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define WRITE_TO_SCREEN(fd, buffer) if(write(fd, buffer, strlen(buffer)) < 0) { perror("write()"); exit(-1); } usleep(300000);

#define TURN_AROUND 2
#define SEMKEY 9000L
#define PERM 0666

// return the semaphore number
#define SEM_NO(sem_no) (sem_no)

// if (sem_op == 0) => waituntil(sem_value == 0)
#define WAIT_ON_ZERO 0
// if (sem_op < 0) => waituntil(sem_value > abs(sem_op)) sem_value = sem_value - abs(sem_op)
#define WAIT_AND_DECREMENT(sem_op) (-sem_op)
// if (sem_op > 0) => sem_value = sem_value + sem_op
#define INCREMENT_SEMAPHORE(sem_op) (sem_op)

/**
 * struct sembuf {
 *  unsigned short   sem_num; -> semaphore number
 *            short   sem_op; -> sempahore operation
 *            short  sem_flg; -> semaphore flags
 * }
 */
static struct sembuf operation_lock[2] = {
    SEM_NO(0), WAIT_ON_ZERO, 0, // wait until sem#0 == 0
    SEM_NO(0), INCREMENT_SEMAPHORE(1), SEM_UNDO // then sem#0 = sem#0 + 1
};


// Although the resource is already locked i.e. sem#0 == 1 and there won't be any waiting (the condition sem#0 >= abs(-1))
// we still explicitly set the IPC_NOWAIT flag to handle this case and return an error
static struct sembuf operation_unlock[1] = {
    SEM_NO(0), WAIT_AND_DECREMENT(1), IPC_NOWAIT | SEM_UNDO
};

int semid = -1; // the semaphore id

#ifdef LOCK_OFF
    void lock_screen() {}
    void unlock_screen() {}
#else

void lock_screen() {
    if (semid < 0) {
        if ((semid = semget(SEMKEY, 1, IPC_CREAT | PERM)) < 0) {
            perror("semget();");
            exit(-1);
        }
    }
    if (semop(semid, &operation_lock[0], 2) < 0) {
        perror("semop()::lock");
        exit(-1);
    }
}

void unlock_screen() {
    if (semop(semid, &operation_unlock[0], 1) < 0) {
        perror("semop()::unlock");
        exit(-1);
    }
}
#endif

#define LOCK_OFF
#define WRITE_WINDOWS
#define WRITE_LINUX

int main() {
    lock_screen();
    system("clear");
    unlock_screen();

#ifdef LOCK_OFF
    printf("Both processes will print on the screen");
#else
    printf("The SCREEN resource is locked");
#endif

    int stdout_fd = fileno(stdout);
    if (stdout_fd < 0) {
        perror("fileno(stdout)");
        exit(-1);
    }

    for (int i = 0; i < TURN_AROUND; i++) {
        // lock file
        lock_screen();

#ifdef WRITE_LINUX
        WRITE_TO_SCREEN(stdout_fd, "L");
        WRITE_TO_SCREEN(stdout_fd, "I");
        WRITE_TO_SCREEN(stdout_fd, "N");
        WRITE_TO_SCREEN(stdout_fd, "U");
        WRITE_TO_SCREEN(stdout_fd, "X");
        WRITE_TO_SCREEN(stdout_fd, "\n");
#endif

#ifdef WRITE_WINDOWS
#ifndef WRITE_LINUX
                        WRITE_TO_SCREEN(stdout_fd, "W");
                        WRITE_TO_SCREEN(stdout_fd, "I");
                        WRITE_TO_SCREEN(stdout_fd, "N");
                        WRITE_TO_SCREEN(stdout_fd, "D");
                        WRITE_TO_SCREEN(stdout_fd, "O");
                        WRITE_TO_SCREEN(stdout_fd, "W");
                        WRITE_TO_SCREEN(stdout_fd, "S");
                        WRITE_TO_SCREEN(stdout_fd, "\n");
#endif
#endif

        // unlock file
        unlock_screen();
    }
    exit(0);
}
