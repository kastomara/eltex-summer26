#ifndef COPY_H
#define COPY_H
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
 
#define BUFFER_SIZE 4096
#define MAX_FILENAME 256
#define PIPE_NAME "my_pipe"
#define TIMEOUT_SEC 5
 
typedef enum {
    MSG_FILE_INFO = 1,
    MSG_FILE_DATA = 2,
    MSG_READY = 3,
    MSG_COMPLETE = 4,
    MSG_ERROR = 5,
    MSG_TERMINATE = 6
} MessageType;
 
typedef struct {
    MessageType type;
    char filename[MAX_FILENAME];
    off_t filesize;
    size_t data_size;
    char data[BUFFER_SIZE];
} Message;
 
void parent_process(int data_pipe, int ack_pipe, char **filenames, int file_count);
void child_process(int data_pipe, int ack_pipe);
int send_message(int fd, const Message *msg);
int receive_message(int fd, Message *msg, int timeout_sec);
void create_copy_filename(const char *original, char *copy, size_t size);
int wait_for_ack(int fd, MessageType expected_type, int timeout_sec);
void write_message(const char *msg);
void write_error(const char *msg);
void write_number(long long num);
void print_usage(const char *progname);
 
#endif  
