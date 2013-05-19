/***********************************************************************
 * This header file defines the structure of the messages. This way we
 * modify the FIFO concept in a message based concept. The same concept
 * is used in System V Unix, and other OSes that use packages for inter
 * process communication.
 **********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_MESSAGE_DATA (4096 - 16)
#define MESSAGE_HEADER_SIZE (sizeof(Mesg) - MAX_MESSAGE_DATA)

typedef struct {
    int msg_len; // will be >= 0
    long msg_type; // server=1L, client=pid
    char msg_data[MAX_MESSAGE_DATA];
} Mesg;

void message_send(int fd, Mesg *messagePtr) {
    int n = MESSAGE_HEADER_SIZE + messagePtr->msg_len;
    if(write(fd, (char *) messagePtr, n) != n) {
        perror("message_send():write_to_fifo");
        exit(-1);
    }
}

int message_receive(int fd, Mesg *messagePtr) {
    int n;
    if((n = read(fd, (char *) messagePtr, MESSAGE_HEADER_SIZE)) == 0) {
        return 0; // EOF
    } else if(n != MESSAGE_HEADER_SIZE) {
        perror("message_receive():bad_message_header");
        exit(-1);
    }

    if((n = messagePtr->msg_len) > 0) {
        if(read(fd, messagePtr->msg_data, n) != n) {
            perror("message_receive():data_read_error");
            exit(-1);
        }
    }
    return n;
}
