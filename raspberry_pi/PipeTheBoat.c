#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    // Command to execute
    const char *cmd = "./your_program";

    // Command to send output via HTTP using curl
    const char *curl_cmd = "curl -X POST -H 'Content-Type: text/plain' --data-binary @- http://example.com/post-endpoint";

    // Create pipe to capture output of the first command
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork a child process
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Child process
        // Close the write end of the pipe
        close(pipefd[1]);

        // Redirect stdin to read from the pipe
        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        // Close the read end of the pipe
        close(pipefd[0]);

        // Execute the curl command
        execlp("sh", "sh", "-c", curl_cmd, NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    } else { // Parent process
        // Close the read end of the pipe
        close(pipefd[0]);

        // Redirect stdout to write to the pipe
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        // Close the write end of the pipe
        close(pipefd[1]);

        // Execute the command to produce output
        execlp("sh", "sh", "-c", cmd, NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    }

    return 0;
}
