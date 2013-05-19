#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include "message.h"

void server(int);

#define FIFO1 "/tmp/fifo.1"
#define PERMISSIONS 0666

void main() {
    int readfd, dummy_writefd;

    // Creating FIFO.1 file for receiving requests from the clients
    if (mknod(FIFO1, S_IFIFO | PERMISSIONS, 0) < 0 && errno != EEXIST) {
        perror("server:mknod('/tmp/fifo.1')");
        exit(-1);
    }

    // Opening the FIFO file for reading the client requests
    if ((readfd = open(FIFO1, O_RDONLY)) < 0) {
        perror("server:open(reading)");
        exit(-1);
    }

    // We open the FIFO file for reading and writing so we can avoid repeatedly opening
    // the file in case there are no clients that would open the FIFO file for writing
    if ((dummy_writefd = open(FIFO1, O_WRONLY)) < 0) {
        perror("server:open(writing)");
        exit(-1);
    }

    printf("The server is waiting for requests.\n");
    server(readfd);

    close(readfd);
    close(dummy_writefd);
    exit(0);
}

void server(int ipcreadfd) {
    Mesg message;
    int n, filefd, ipcwritefd, childpid;
    char errmsg[256], fifo_name[256];

    signal(SIGCHLD, SIG_IGN); // ignoring SIGCHLD signals so the so child processes don't become zombies

    // the parent process waits for a request from the client
    while (1) {
        // the server reads the name of the file that it should open
        if ((n = message_receive(ipcreadfd, &message)) <= 0) {
            printf("server:filename_read_error");
            exit(-1);
        }
        message.msg_data[n] = '\0';

        if ((childpid = fork()) < 0) {
            printf("Can't spawn a child process\n");
            exit(-1);
        } else if (childpid == 0) {
            // create the FIFO name type based on the message.msg_type
            sprintf(fifo_name, "/tmp/fifo.%ld", message.msg_type);

            // open the FIFO file for sending a response to the client
            if ((ipcwritefd = open(fifo_name, O_WRONLY)) < 0) {
                perror("server:open(writing)");
                exit(-1);
            }
            printf("The server will respond over the FIFO file: %s\n", fifo_name);

            // Opening the file that we'll send back to the client, the filename is stored in message.msg_data
            if ((filefd = open(message.msg_data, 0)) < 0) {
                // return error back to the client
                sprintf(errmsg, ":server:can't open file: %s\n", strerror(errno));
                strcat(message.msg_data, errmsg);
                message.msg_len = strlen(message.msg_data);
                message_send(ipcwritefd, &message);
            } else {
                message.msg_type = 1L; // response from the server
                // read the data from the file and send it to the client
                while ((n = read(filefd, message.msg_data, MAX_MESSAGE_DATA)) > 0) {
                    message.msg_len = n;
                    message_send(ipcwritefd, &message);
                }
                close(filefd);
                if (n < 0) {
                    printf("server:read error file");
                    exit(-1);
                }
            }

            // finish the file transfer
            message.msg_len = 0; // EOF marker
            message_send(ipcwritefd, &message);
            close(ipcwritefd);
            exit(0); // the child process finished processing the request
        }
    }
}
