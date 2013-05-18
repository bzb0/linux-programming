/*************************************************************************************
 * This program outputs all semaphores in the system and gives the opportunity to the
 * user to destroy an existing semaphore. The semaphores are system variables and
 * are visible to all processes, not only the process that created them. They also
 * continue to exist after the process dies, so they need to be destroyed afterwards.
 ************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <signal.h>
#include <time.h>

#define SEMKEY 9000L

int semid;
struct semid_ds buffer_ds;

// we have to define "semun" & "info_buffer"
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
} arg;

#define info_buffer arg.buf

void main() {
    // get semaphore
    if ((semid = semget(SEMKEY, 1, 0)) < 0) {
        perror("semget()");
        exit(-1);
    }

    arg.buf = &buffer_ds;
    if (semctl(semid, 0, IPC_STAT, arg) < 0) {
        perror("semctl()");
        exit(-1);
    }

    printf("********************************************\n");
    printf("1. General info for the set of semaphores:\n");
    printf("********************************************\n");
    printf("Number of semaphores in the set: %ld\n", info_buffer->sem_nsems);
    printf("Time since last sem_op call: %s", ctime(&info_buffer->sem_otime));
    printf("Time since last change: %s\n", ctime(&info_buffer->sem_ctime));
    printf("\n");

    printf("*************************\n");
    printf("2. IPC allowing struct:\n");
    printf("*************************\n");
    printf("Owner's user ID: %d\n", info_buffer->sem_perm.uid);
    printf("Owner's group ID: %d\n", info_buffer->sem_perm.gid);
    printf("Creator's user ID: %d\n", info_buffer->sem_perm.cuid);
    printf("Creator's group ID: %d\n", info_buffer->sem_perm.cgid);
    printf("Access modes: %o\n", info_buffer->sem_perm.mode);
    printf("Slot usage sequence number: %d\n", info_buffer->sem_perm.__seq);
    printf("key_t identifier: %d\n", info_buffer->sem_perm.__key);
    printf("\n");

    printf("***********************************************\n");
    printf("3. Overview of all the sempahores in the set:\n");
    printf("***********************************************\n");
    for (int i = 0; i < info_buffer->sem_nsems; i++) {
        printf("Semaphore value: %d\n", semctl(semid, i, GETVAL, 0));
        printf("PID of the last operation: %d\n", semctl(semid, i, GETPID, 0));
        printf("Number of waiting processes 'semval > cval': %d\n", semctl(semid, i, GETNCNT, 0));
        printf("Number of (conditional) waiting processes  'semval == 0': %d\n", semctl(semid, i, GETZCNT, 0));
    }

    printf("Enter <ENTER> to destory a semaphore or Ctrl+C to exit.\n");
    getchar();
    arg.val = 0;
    semctl(SEMKEY, 0, IPC_RMID, arg);
    exit(0);
}
