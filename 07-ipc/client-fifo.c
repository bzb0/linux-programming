#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include "message.h"

void client(int);

#define FIFO1 "/tmp/fifo.1"
#define PERMISSIONS 0666

Mesg message;
char fifo_name[256];

void main() {
    int readfd, writefd;
    char errmsg[256];

    printf("Client with PID: %d\n", getpid());

    // open the FIFO file (/tmp/fifo.1) for sending the request. (Should already be created by the server)
    if((writefd = open(FIFO1, O_WRONLY)) < 0) {
        perror("client:open(writing)");
        exit(-1);
    }

    // The client will receive the reply from the server over this FIFO file
    sprintf(fifo_name, "/tmp/fifo.%ld", (long)getpid());

    // Create a FIFO file for sending the reply
    if(mknod(fifo_name, S_IFIFO | PERMISSIONS, 0) < 0 && errno != EEXIST) {
        sprintf(errmsg, "client:mknod():%s\n", fifo_name);
        perror(errmsg);
        exit(-1);
    }

    printf("Client created FIFO file: %s\n", fifo_name);
    client(writefd);
    close(writefd);

    // The client finished so we can delete the FIFO file
    if(unlink(fifo_name) < 0) {
        perror("client:unlink()");
        exit(-1);
    }
    printf("Client deleted the FIFO file: %s\n", fifo_name);
    exit(0);
}

void client(int ipcwritefd) {
    // Read the filename from STDIN and send a request to the server
    printf("Enter the filename that will printed out: ");
    if(fgets(message.msg_data, MAX_MESSAGE_DATA, stdin) == NULL) {
        perror("client:fgets()");
        exit(-1);
    }

    int n = strlen(message.msg_data);
    n = message.msg_data[n-1] == '\n' ? n - 1 : n;
    message.msg_len = n;
    message.msg_type = (long)getpid();

    message_send(ipcwritefd, &message);

    int ipcreadfd;
    // open the client FIFO file for reading the response from the server
    if( (ipcreadfd = open(fifo_name, O_RDONLY))  < 0) {
        perror("client:open(reading");
        exit(-1);
    }

    // receive data from the server
    while((n = message_receive(ipcreadfd, &message)) > 0) {
        printf("Received server response via FIFO file: %s\n", fifo_name);
        sleep(1);
        // writing the message to STDOUT
        if(write(1, message.msg_data, n) != n) {
            perror("client:write(stdout");
            exit(-1);
        }
    }

    if(n < 0) {
        perror("client:message_receive()");
        exit(-1);
    }
    close(ipcreadfd);
}
