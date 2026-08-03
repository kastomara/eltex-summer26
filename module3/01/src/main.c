#include "copy.h"

void print_usage(const char *progname) {
    write_message("Usage: ");
    write_message(progname);
    write_message(" [-p pipe_name] file1 [file2 ...]\n");
}

int main(int argc, char *argv[]) {
    int data_pipe[2];
    int ack_pipe[2];
    pid_t pid;
    int use_named_pipe = 0;
    const char *pipe_name = PIPE_NAME;
    int file_start_index = 1;
    int data_fd, ack_fd;
    int status;
    
    if (argc < 2) {
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    
    if (argc >= 3 && strcmp(argv[1], "-p") == 0) {
        use_named_pipe = 1;
        pipe_name = argv[2];
        file_start_index = 3;
        
        if (argc < 4) {
            write_error("Error: no files specified for copying\n");
            print_usage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    
    int file_count = argc - file_start_index;
    char **filenames = &argv[file_start_index];
    
    write_message("=== File Copying Program ===\n");
    write_message("Number of files: ");
    write_number(file_count);
    write_message("\n");
    write_message("Using: ");
    write_message(use_named_pipe ? "named pipe" : "unnamed pipe");
    write_message("\n");
    
    if (use_named_pipe) {
        char data_pipe_name[MAX_FILENAME];
        char ack_pipe_name[MAX_FILENAME];
        snprintf(data_pipe_name, sizeof(data_pipe_name), "%s_data", pipe_name);
        snprintf(ack_pipe_name, sizeof(ack_pipe_name), "%s_ack", pipe_name);
        
        if (mkfifo(data_pipe_name, 0666) == -1 && errno != EEXIST) {
            write_error("Error creating data pipe: ");
            write_error(strerror(errno));
            write_error("\n");
            exit(EXIT_FAILURE);
        }
        if (mkfifo(ack_pipe_name, 0666) == -1 && errno != EEXIST) {
            write_error("Error creating acknowledgement pipe: ");
            write_error(strerror(errno));
            write_error("\n");
            unlink(data_pipe_name);
            exit(EXIT_FAILURE);
        }
        
        pid = fork();
        if (pid == -1) {
            write_error("Error creating process: ");
            write_error(strerror(errno));
            write_error("\n");
            unlink(data_pipe_name);
            unlink(ack_pipe_name);
            exit(EXIT_FAILURE);
        }
        
        if (pid == 0) {
            data_fd = open(data_pipe_name, O_RDONLY);
            ack_fd = open(ack_pipe_name, O_WRONLY);
            
            if (data_fd == -1 || ack_fd == -1) {
                write_error("Error opening pipes\n");
                exit(EXIT_FAILURE);
            }
            
            child_process(data_fd, ack_fd);
            
            close(data_fd);
            close(ack_fd);
            exit(EXIT_SUCCESS);
        } else {
            data_fd = open(data_pipe_name, O_WRONLY);
            ack_fd = open(ack_pipe_name, O_RDONLY);
            
            if (data_fd == -1 || ack_fd == -1) {
                write_error("Error opening pipes\n");
                kill(pid, SIGTERM);
                unlink(data_pipe_name);
                unlink(ack_pipe_name);
                exit(EXIT_FAILURE);
            }
            
            parent_process(data_fd, ack_fd, filenames, file_count);
            
            close(data_fd);
            close(ack_fd);
            
            unlink(data_pipe_name);
            unlink(ack_pipe_name);
            
            waitpid(pid, &status, 0);
            
            write_message("\nChild process exited with code: ");
            write_number(WEXITSTATUS(status));
            write_message("\n");
        }
    } else {
        if (pipe(data_pipe) == -1 || pipe(ack_pipe) == -1) {
            write_error("Error creating pipes: ");
            write_error(strerror(errno));
            write_error("\n");
            exit(EXIT_FAILURE);
        }
        
        pid = fork();
        if (pid == -1) {
            write_error("Error creating process: ");
            write_error(strerror(errno));
            write_error("\n");
            exit(EXIT_FAILURE);
        }
        
        if (pid == 0) {
            close(data_pipe[1]);
            close(ack_pipe[0]);
            
            child_process(data_pipe[0], ack_pipe[1]);
            
            close(data_pipe[0]);
            close(ack_pipe[1]);
            exit(EXIT_SUCCESS);
        } else {
            close(data_pipe[0]);
            close(ack_pipe[1]);
            
            parent_process(data_pipe[1], ack_pipe[0], filenames, file_count);
            
            close(data_pipe[1]);
            close(ack_pipe[0]);
            
            waitpid(pid, &status, 0);
            
            write_message("\nChild process exited with code: ");
            write_number(WEXITSTATUS(status));
            write_message("\n");
        }
    }
    
    write_message("=== Program finished ===\n");
    return EXIT_SUCCESS;
}