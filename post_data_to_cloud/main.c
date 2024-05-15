#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void handle_signal(int sig) {
    // Send the signal to the entire process group
    kill(0, sig);
}

/**
 * Function executes the AIS catcher and redirects its standard output to a python
 * listener script to send the request.
 */
int main() {
    int pipe_fd[2];
    pid_t cpid;

    // Create a pipe
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Set up a signal handler for terminating the process group
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // Create a new process group for the children
    if (setpgid(0, 0) == -1) {
        perror("setpgid");
        exit(EXIT_FAILURE);
    }

    // Fork a child process to compile and run the C program
    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) {  // Child process
        // Move this process to the same process group as the parent
        if (setpgid(0, 0) == -1) {
            perror("setpgid (child)");
            exit(EXIT_FAILURE);
        }

        // Close the read end of the pipe
        close(pipe_fd[0]);

        // Redirect stdout to the write end of the pipe
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);

        // Compile the child.c program
        execlp("gcc", "gcc", "-o", "child", "ais_program.c", NULL);

        // If execlp to gcc fails
        perror("execlp");
        exit(EXIT_FAILURE);
    } else {  // Parent process
        // Wait for the compilation to finish
        wait(NULL);

        // Fork another child process to run the compiled C program
        cpid = fork();
        if (cpid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (cpid == 0) {  // Child process for executing the compiled C program
            // Move this process to the same process group as the parent
            if (setpgid(0, 0) == -1) {
                perror("setpgid (child)");
                exit(EXIT_FAILURE);
            }

            // Close the read end of the pipe
            close(pipe_fd[0]);

            // Redirect stdout to the write end of the pipe
            dup2(pipe_fd[1], STDOUT_FILENO);
            close(pipe_fd[1]);

            // Execute the compiled child program
            execlp("./child", "./child", NULL);

            // If execlp fails
            perror("execlp");
            exit(EXIT_FAILURE);
        } else {  // Parent process
            // Close the write end of the pipe
            close(pipe_fd[1]);

            // Fork another child process to run the Python script
            pid_t py_cpid = fork();
            if (py_cpid == 0) {  // Child process for Python script
                // Move this process to the same process group as the parent
                if (setpgid(0, 0) == -1) {
                    perror("setpgid (child)");
                    exit(EXIT_FAILURE);
                }

                // Redirect stdin to the read end of the pipe
                dup2(pipe_fd[0], STDIN_FILENO);
                close(pipe_fd[0]);

                // Execute the Python script
                execlp("python3", "python3", "posting_process.py", NULL);

                // If execlp fails
                perror("execlp");
                exit(EXIT_FAILURE);
            } else if (py_cpid > 0) {  // Parent process
                // Move this process to the same process group as the parent
                if (setpgid(0, 0) == -1) {
                    perror("setpgid (parent)");
                    exit(EXIT_FAILURE);
                }

                // Close the read end of the pipe
                close(pipe_fd[0]);

                // Wait for both child processes to finish
                wait(NULL);
                wait(NULL);
            } else {
                perror("fork");
                exit(EXIT_FAILURE);
            }
        }
    }

    return 0;
}