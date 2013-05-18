#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define MUTEX 0
#define EMPTY 1
#define FULL 2

#define PRODUCER 0
#define CONSUMER 1

#define SEMNUM 3
#define OPCODE 0

int semid; // the semaphore id

// The declaration below corresponds with "unsigned short *array" from the "semun" union
unsigned short initvalues[3];

// Increase or decrease the value of the semaphore with SEMNUM depending on the OPCODE and perform an UNDO on exit
// SEMNUM values 0, 1, 2:
// - semnum(mutex) = 0
// - semnum(empty) = 1
// - semnum(full) = 2
// OPCODE values: -1, 1
// these values are set by the functions semaphore_signal() and semaphore_wait()
struct sembuf op_op[1] = {
    SEMNUM, OPCODE, SEM_UNDO
};

#ifndef SEM_OFF
// general function for semaphores. It covers sem_wait and sem_signal. The opcode can't be 0
void semaphore_operate(int sem_id, unsigned short sem_no, short sem_opcode) {
    if (sem_no > 2) {
        printf("Allowed sem_no values are: 0, 1, 2\n");
        exit(-1);
    }
    if (sem_opcode == 0) {
        printf("Allowed sem_opcode values are: 1, -1\n");
        exit(-1);
    }
    op_op[0].sem_op = sem_opcode; // operation number
    op_op[0].sem_num = sem_no; // semaphore number

    if (semop(sem_id, &op_op[0], 1) < 0) {
        perror("semop()");
        exit(-1);
    }
}
#else
    void sem_oper(int, unsigned short, short) {
    }
#endif

// Block until the semaphore value gets bigger than 0, then decrease the value by 1 and return
// This is same as Dijkstra's operation P and Tanenbaum's operation DOWN
void semaphore_wait(int sem_id, unsigned short sem_no) {
    semaphore_operate(sem_id, sem_no, -1);
}

// Increase the value of the semaphore by 1
// This is the same as Dijkstra's operation V and Tanenbaum's operation DOWN
void semaphore_signal(int sem_id, unsigned short sem_no) {
    semaphore_operate(sem_id, sem_no, 1);
}

void consumer() {
    char message[] = "Removing item by the consumer";
    while (1) {
        semaphore_wait(semid, FULL); // wait until there's something in the buffer DEC(count(items))

        semaphore_wait(semid, MUTEX); // entering critical section
        for (short i = 0; i < strlen(message); i++) {
            printf("%c", message[i]); // removing item from the buffer
            usleep(200000);
        }
        printf("\r");
        semaphore_signal(semid, MUTEX); // leaving critical section

        semaphore_signal(semid, EMPTY); // send signal to the producer that there's an empty space in the buffer
    }
}

void producer() {
    char message[] = "Inserting item by the producer";
    while (1) {
        semaphore_wait(semid, EMPTY); // wait until there's space in the buffer DEC(count(empty_space))

        semaphore_wait(semid, MUTEX); // entering critical section
        for (short i = 0; i < strlen(message); i++) {
            printf("%c", message[i]);
            usleep(200000);
        }
        printf("\r");
        semaphore_signal(semid, MUTEX); // leaving critical section

        semaphore_signal(semid, FULL); // send signal to the consumer that there's an item to be consumed INC(count(items))
    }
}

void main() {
    // making STDOUT unbuffered
    setvbuf(stdout, NULL, _IONBF, BUFSIZ);
    system("clear");
    printf("Press Ctrl+C for terminating the producer & consumer.\n");

    // Creating 3 semaphores that correspond
    // typedef int semaphore;
    // semaphore mutex = 1;
    // semaphore empty = N;
    // semaphore full = 0;
    if ((semid = semget(8000L, 3, 0666 | IPC_CREAT)) < 0) {
        perror("semget()");
        exit(-1);
    }

    initvalues[0] = 1; // mutex
    initvalues[1] = 1; // empty
    initvalues[2] = 0; // full

    // semnum is ignored in this context
    if (semctl(semid, 0, SETALL, initvalues) < 0) {
        perror("semctl()");
        exit(-1);
    }

    int pid = 0;
    if ((pid = fork()) == -1) {
        printf("Can't spawn a child process\n");
        exit(-1);
    } else if (pid == 0) {
        consumer();
        exit(0);
    } else {
        producer();
        exit(0);
    }
}
