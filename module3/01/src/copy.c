#include "copy.h"
 
void write_message(const char *msg) {
    if (msg) {
        write(STDOUT_FILENO, msg, strlen(msg));
    }
}
 
void write_error(const char *msg) {
    if (msg) {
        write(STDERR_FILENO, msg, strlen(msg));
    }
}
 
void write_number(long long num) {
    char buffer[32];
    int len = 0;
    
    if (num == 0) {
        write_message("0");
        return;
    }
    
    if (num < 0) {
        write_message("-");
        num = -num;
    }
    
    char temp[32];
    int i = 0;
    while (num > 0) {
        temp[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    while (i > 0) {
        buffer[len++] = temp[--i];
    }
    buffer[len] = '\0';
    
    write_message(buffer);
}

static ssize_t write_all(int fd, const void *buf, size_t count) {
    const char *p = buf;
    size_t total = 0;
    while (total < count) {
        ssize_t n = write(fd, p + total, count - total);
        if (n == -1) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        total += (size_t)n;
    }
    return (ssize_t)total;
}

static ssize_t read_all(int fd, void *buf, size_t count) {
    char *p = buf;
    size_t total = 0;
    while (total < count) {
        ssize_t n = read(fd, p + total, count - total);
        if (n == -1) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        if (n == 0) {
            break;
        }
        total += (size_t)n;
    }
    return (ssize_t)total;
}
 
int send_message(int fd, const Message *msg) {
    ssize_t bytes_written = write_all(fd, msg, sizeof(Message));
    if (bytes_written != (ssize_t)sizeof(Message)) {
        return -1;
    }
    return 0;
}
 
int receive_message(int fd, Message *msg, int timeout_sec) {
    fd_set read_fds;
    struct timeval tv;
    
    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);
    
    tv.tv_sec = timeout_sec;
    tv.tv_usec = 0;
    
    int ret = select(fd + 1, &read_fds, NULL, NULL, &tv);
    if (ret == -1) {
        write_error("select error\n");
        return -1;
    } else if (ret == 0) {
        write_error("Timeout waiting for message\n");
        return -1;
    }
    
    ssize_t bytes_read = read_all(fd, msg, sizeof(Message));
    if (bytes_read != (ssize_t)sizeof(Message)) {
        return -1;
    }
    
    return 0;
}
 
int wait_for_ack(int fd, MessageType expected_type, int timeout_sec) {
    Message msg;
    if (receive_message(fd, &msg, timeout_sec) == -1) {
        return -1;
    }
    
    if (msg.type == MSG_ERROR) {
        write_error("Received error from child\n");
        return -1;
    }
    
    if (msg.type != expected_type) {
        write_error("Expected message type: ");
        write_number(expected_type);
        write_error(", received: ");
        write_number(msg.type);
        write_error("\n");
        return -1;
    }
    
    return 0;
}
 
void create_copy_filename(const char *original, char *copy, size_t size) {
    snprintf(copy, size, "%s.copy", original);
}
 
off_t get_file_size(const char *filename) {
    struct stat st;
    if (stat(filename, &st) == -1) {
        return -1;
    }
    return st.st_size;
}
 
int send_file(int data_pipe, const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        write_error("Error opening file for reading: ");
        write_error(filename);
        write_error("\n");
        return -1;
    }
    
    Message msg;
    msg.type = MSG_FILE_DATA;
    msg.data_size = 0;
    
    ssize_t bytes_read;
    while ((bytes_read = read(fd, msg.data, BUFFER_SIZE)) > 0) {
        msg.data_size = bytes_read;
        if (send_message(data_pipe, &msg) == -1) {
            write_error("Error sending data\n");
            close(fd);
            return -1;
        }
    }
    
    if (bytes_read == -1) {
        write_error("Error reading file\n");
        close(fd);
        return -1;
    }
    
    close(fd);
    return 0;
}
 
int receive_and_write_file(int data_pipe, int ack_pipe) {
    Message msg;
    
    int ret = receive_message(data_pipe, &msg, TIMEOUT_SEC);
    if (ret == -1) {
        return -1;
    }
    
    if (msg.type == MSG_TERMINATE) {
        return 0;
    }
    
    if (msg.type == MSG_ERROR) {
        write_error("Error received from parent for file: ");
        write_error(msg.filename);
        write_error("\n");
        return -1;
    }
    
    if (msg.type != MSG_FILE_INFO) {
        write_error("Expected file info, received type: ");
        write_number(msg.type);
        write_error("\n");
        return -1;
    }
    
    char orig_filename[MAX_FILENAME];
    strncpy(orig_filename, msg.filename, MAX_FILENAME - 1);
    orig_filename[MAX_FILENAME - 1] = '\0';
 
    off_t expected_size = msg.filesize;
 
    char copy_filename[MAX_FILENAME + 10];
    create_copy_filename(orig_filename, copy_filename, sizeof(copy_filename));
    
    int fd = open(copy_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        write_error("Error creating copy file: ");
        write_error(copy_filename);
        write_error("\n");
        return -1;
    }
    
    Message ack_msg;
    ack_msg.type = MSG_READY;
    if (send_message(ack_pipe, &ack_msg) == -1) {
        write_error("Error sending ready acknowledgement\n");
        close(fd);
        return -1;
    }
    
    off_t total_received = 0;
    while (total_received < expected_size) {
        ret = receive_message(data_pipe, &msg, TIMEOUT_SEC);
        if (ret == -1) {
            write_error("Error receiving file data\n");
            close(fd);
            return -1;
        }
        
        if (msg.type == MSG_ERROR) {
            write_error("Error receiving file: ");
            write_error(msg.filename);
            write_error("\n");
            close(fd);
            return -1;
        }
 
        if (msg.type == MSG_TERMINATE) {
            write_error("Unexpected termination during file transfer\n");
            close(fd);
            return -1;
        }
        
        if (msg.type != MSG_FILE_DATA) {
            write_error("Expected file data, received type: ");
            write_number(msg.type);
            write_error("\n");
            close(fd);
            return -1;
        }
        
        ssize_t bytes_written = write(fd, msg.data, msg.data_size);
        if (bytes_written != (ssize_t)msg.data_size) {
            write_error("Error writing to file\n");
            close(fd);
            return -1;
        }
        
        total_received += bytes_written;
    }
    
    close(fd);
    
    write_message("File '");
    write_message(orig_filename);
    write_message("' copied to '");
    write_message(copy_filename);
    write_message("' (");
    write_number(total_received);
    write_message(" bytes)\n");
    
    Message complete_msg;
    complete_msg.type = MSG_COMPLETE;
    if (send_message(ack_pipe, &complete_msg) == -1) {
        write_error("Error sending complete acknowledgement\n");
        return -1;
    }
    
    return 1;
}
 
void parent_process(int data_pipe, int ack_pipe, char **filenames, int file_count) {
    char pid_str[32];
    snprintf(pid_str, sizeof(pid_str), "%d", getpid());
    
    write_message("Parent process (PID: ");
    write_message(pid_str);
    write_message(") started\n");
    
    for (int i = 0; i < file_count; i++) {
        const char *filename = filenames[i];
        
        write_message("\n[Parent] Processing file: ");
        write_message(filename);
        write_message("\n");
        
        off_t filesize = get_file_size(filename);
        if (filesize == -1) {
            write_error("[Parent] Error: file '");
            write_error(filename);
            write_error("' does not exist\n");
            /* The child was never told about this file, so it is not
             * expecting anything for it — simply skip to the next file
             * instead of pushing a message into the pipe that would
             * desynchronize the parent/child protocol. */
            continue;
        }
        
        Message info_msg;
        info_msg.type = MSG_FILE_INFO;
        strncpy(info_msg.filename, filename, MAX_FILENAME - 1);
        info_msg.filename[MAX_FILENAME - 1] = '\0';
        info_msg.filesize = filesize;
        info_msg.data_size = 0;
        
        if (send_message(data_pipe, &info_msg) == -1) {
            write_error("[Parent] Error sending file info\n");
            continue;
        }
        
        write_message("[Parent] Sent info: ");
        write_message(filename);
        write_message(" (");
        write_number(filesize);
        write_message(" bytes)\n");
        
        write_message("[Parent] Waiting for acknowledgement...\n");
        if (wait_for_ack(ack_pipe, MSG_READY, TIMEOUT_SEC) == -1) {
            write_error("[Parent] No acknowledgement received for ");
            write_error(filename);
            write_error("\n");
            continue;
        }
        write_message("[Parent] Received ready acknowledgement\n");
        
        write_message("[Parent] Sending content of ");
        write_message(filename);
        write_message("...\n");
        
        if (send_file(data_pipe, filename) == -1) {
            write_error("[Parent] Error sending file ");
            write_error(filename);
            write_error("\n");
            continue;
        }
        write_message("[Parent] File ");
        write_message(filename);
        write_message(" sent\n");
        
        write_message("[Parent] Waiting for complete acknowledgement...\n");
        if (wait_for_ack(ack_pipe, MSG_COMPLETE, TIMEOUT_SEC) == -1) {
            write_error("[Parent] No complete acknowledgement received for ");
            write_error(filename);
            write_error("\n");
            continue;
        }
        write_message("[Parent] Received complete acknowledgement\n");
    }
    
    write_message("\n[Parent] Sending termination signal...\n");
    Message term_msg;
    term_msg.type = MSG_TERMINATE;
    term_msg.filesize = 0;
    term_msg.data_size = 0;
    send_message(data_pipe, &term_msg);
    
    Message ack;
    if (receive_message(ack_pipe, &ack, TIMEOUT_SEC) == 0 && ack.type == MSG_TERMINATE) {
        write_message("[Parent] Child confirmed termination\n");
    }
    
    write_message("[Parent] Finishing work\n");
}
 
void child_process(int data_pipe, int ack_pipe) {
    char pid_str[32];
    snprintf(pid_str, sizeof(pid_str), "%d", getpid());
    
    write_message("Child process (PID: ");
    write_message(pid_str);
    write_message(") started\n");
    write_message("Waiting for files...\n");
    
    while (1) {
        int result = receive_and_write_file(data_pipe, ack_pipe);
        
        if (result == 0) {
            write_message("[Child] Received termination signal\n");
            Message term_ack;
            term_ack.type = MSG_TERMINATE;
            send_message(ack_pipe, &term_ack);
            break;
        } else if (result == -1) {
            write_error("[Child] Error receiving file\n");
            Message error_msg;
            error_msg.type = MSG_ERROR;
            send_message(ack_pipe, &error_msg);
            break;
        }
    }
    
    write_message("[Child] Finishing work\n");
}
