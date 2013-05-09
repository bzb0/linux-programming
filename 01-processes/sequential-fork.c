/****************************************************************************************
 * This program demonstrates process creation in a chain sequence, where the parent
 * process always waits for its child process to complete. This is a hierarchical
 * processing structure that is called expand&reduce. The first created process will
 * finish when every process before it has finished working. Here the use of the
 * wait function is demonstrated, in which the parent waits on the child process to end.
 ***************************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

void interpret_status(int status);

int child_pid, child_cnt = 0;

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("Usage: sequential_fork <NUM_CHILD_PROCESSES>\n");
        return -1;
    }

    int status;
    int how_many_childs = atoi(argv[1]);
    if (how_many_childs <= 0) return -1;

    signal(SIGINT, SIG_IGN);
    setvbuf(stdout, NULL, _IONBF, BUFSIZ);

    while (1) {
        printf("[PID=%d, PPID=%d] The process spawning has started ... \n", getpid(), getppid());
        sleep(1);
        switch (child_pid = fork()) {
            case -1:
                if (errno == EAGAIN) {
                    printf("[PARENT][PID=%d, PPID=%d] There's not enough memory for crearing a child process.\n", getpid(), getppid());
                }
                exit(-1);
            case 0:
                printf("[CHILD][PID=%d, PPID=%d] Process #%d was successfully spawned.\n", getpid(), getppid(), child_cnt);
                child_cnt++;
                if (child_cnt == how_many_childs) {
                    printf("[CHILD][PID=%d, PPID=%d] The unwinding of the spawned processes has started. "
                           "I'm the last created process. I'll sleep for 3sec and I'll exit normally (Ctrl+C is ignored).\n", getpid(), getppid());
                    sleep(3);
                    exit(getpid());
                }
                break;
            default:
                printf("[PARENT][PID=%d, PPID=%d] I'm waiting for the child process to end.\n", getpid(), getppid());
                wait(&status);
                interpret_status(status);
                printf("[PARENT][PID=%d, PPID=%d] I'll sleep for 5 sec. If you don't interrupt me with Ctrl+C I'll exit normally. "
                       "Otherwise my parent will get my termination signal.\n", getpid(), getppid());

                if (child_cnt == 0) {
                    printf("[PARENT][PID=%d, PPID=%d] I created the Child processes and now I'll return to the terminal which is waiting for me.\n",
                           getpid(), getppid());
                }

                signal(SIGINT, SIG_DFL);
                sleep(5);
                exit(getpid());
        }
    }
}

void interpret_status(int status) {
    unsigned char term_signal = 0x7F & status;
    unsigned char core_dump_bit = (0x80 & status) ? 1 : 0;
    unsigned char ret_code = (0xFF00 & status) >> 8;

    if (term_signal) {
        printf("[STATUS HANDLER][PID=%d, PPID=%d] The process was terminated because of SIGNAL=%d "
               "and%sperformed CORE_DUMP of its 'image'.\n", child_pid, getpid(), term_signal, core_dump_bit ? " " : " didn't ");
    } else {
        printf("[STATUS HANDLER][PID=%d, PPID=%d] The process terminated with exit() and RETURN CODE=%d\n", child_pid, getpid(), ret_code);
    }
}
