/********************************************************************************************
 * This is an implementation of the Peterson's algorithm for addressing the critical section
 * problem with two processes. The Peterson's algorithm also solves the problem when one of
 * the processes dies. The algorithm uses a lock variable and strict alternation.
 *******************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0
#define N 2      // number of processes

int pid1, pid2, resource, turn, counter1 = 5;
char resource_var, turn_var;

#define CREATE_RESOURCE resource = open("resource", O_RDWR | O_CREAT | O_TRUNC, 0666);  if (resource == -1) { perror("Error creating resource file"); exit(-1); }
#define OPEN_RESOURCE resource = open("resource", O_RDWR , 0666); if (resource == -1 ) { perror("Error opening resource file"); exit(- 1 ); }
#define WRITE_RESOURCE(valueP) write(resource, valueP, 1); lseek(resource, 0, L_SET)
#define READ_RESOURCE(valueP) read(resource, valueP, 1); lseek(resource, 0, L_SET);

#define CREATE_TURN turn = open("turn", O_RDWR | O_CREAT | O_TRUNC, 0666 ); if(turn == -1) { perror("Error creating TURN variable file"); exit(-1 ); }
#define OPEN_TURN turn = open("turn" , O_RDWR, 0666); if(turn == -1) { perror("Error opening TURN variable file"); exit(-1); }
#define WRITE_TURN(valueP) write(turn, valueP, 1); lseek(turn, 0, L_SET);
#define READ_TURN(valueP) read(turn, valueP, 1); lseek(turn, 0, L_SET);

// Sets if the process wants to enter the critical section
void INTERESTED(int process, int flag) {
    char file_name[32];
    static int fd;

    strcpy(file_name, "interested");
    strcat(file_name, process ? "1" : "0");

    if (flag == TRUE) {
        fd = creat(file_name, 0666);
        if (fd == -1) {
            perror(file_name);
            exit(-1);
        }
    } else {
        int err = unlink(file_name);
        if (err == -1) {
            perror(file_name);
            exit(-1);
        }
        close(fd);
    }
}

// Checks if the process is interested
int TEST_INTERESTED(int process) {
    char file_name[32];
    struct stat StatBuffer;

    strcpy(file_name, "interested");
    strcat(file_name, process ? "1" : "0");

    if (stat(file_name, &StatBuffer) == -1) {
        return FALSE;
    } else {
        return TRUE;
    }
}

// Sets the turn variable to the process number
void TURN_TO(int process) {
    WRITE_TURN(&process); // turn = process
}

// Checks the TURN variable
int TEST_TURN(int process) {
    READ_TURN(&turn_var);

    if (turn_var == process) {
        return TRUE;
    } else {
        return FALSE;
    }
}

// Wait for permission to enter critical section
void enter_critical_section(int process) {
    int other = 1 - process; // other process number

    INTERESTED(process, TRUE);
    TURN_TO(process); // turn = process

    // while(turn == process && interested[other] == TRUE);
    while (TEST_TURN(process) && TEST_INTERESTED(other)) {
        // wait
        printf("WAIТ(%d)\n", process);
        sleep(2);
    }
}

// Leaves the critical section
void leave_critical_section(int process) {
    INTERESTED(process, FALSE); // interested[process] = FALSE
}

void main() {
    resource_var = 0;
    turn_var = 0;

    CREATE_RESOURCE;
    WRITE_RESOURCE(&resource_var);
    CREATE_TURN;

    switch (pid1 = fork()) {
        case -1:
            printf("Can't spawn process 1\n");
            exit(-1);
        case 0:
            OPEN_RESOURCE;
            OPEN_TURN;
            while (counter1--) {
                enter_critical_section(0);
                printf("--> IN(0)\n");

                READ_RESOURCE(&resource_var);
                resource_var++;
                WRITE_RESOURCE(&resource_var);
                sleep(2);
                READ_RESOURCE(&resource_var);
                resource_var--;
                WRITE_RESOURCE(&resource_var);
                sleep(2);
                leave_critical_section(0);
            }
            printf("\nProcess 0 terminates...\n");
            printf("Press СTRL+C for the other child process to stop after monitoring ...\n");
            close(turn);
            close(resource);
            break;
        default:
            switch (pid2 = fork()) {
                case -1:
                    printf("Can't spawn process 2\n");
                    exit(-1);
                case 0:
                    OPEN_RESOURCE;
                    OPEN_TURN;
                    while (1) {
                        enter_critical_section(1);
                        printf("--> IN(1)\n");
                        READ_RESOURCE(&resource_var);
                        resource_var++;
                        WRITE_RESOURCE(&resource_var);
                        sleep(2);
                        READ_RESOURCE(&resource_var);
                        resource_var--;
                        WRITE_RESOURCE(&resource_var);
                        sleep(2);
                        leave_critical_section(1);
                    }
                    break;
                default:
                    close(turn);
                    while (1) {
                        READ_RESOURCE(&resource_var);
                        printf("Inspect resource: %d\n", resource_var);
                        sleep(2);
                        if (resource_var != 0 && resource_var != 1) {
                            printf("Error occured...\n");
                            kill(pid1, SIGTERM);
                            kill(pid2, SIGTERM);
                            exit(-1);
                        }
                    }
            }
    }
}
