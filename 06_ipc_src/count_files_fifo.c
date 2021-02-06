/* count files by running ls and redirecting its output to wc -l */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* Wait for child process and report on error.
Returns zero on success and -1 on error */
int wait_report(pid_t pid);

int main() {
    const char *fifopath = "fifo";
    if (-1 == mkfifo(fifopath, 0666)) {
        perror("mkfifo");
        return EXIT_FAILURE;
    }

    pid_t pid[2];
    pid[0] = fork();
    if (pid[0] == 0) {/* child branch */
        int fd = open(fifopath, O_WRONLY);
        if (fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        /* redirect stdout to FIFO fd */
        if (-1 == dup2(fd, STDOUT_FILENO)) {
            perror("dup2");
            close(fd);
            exit(EXIT_FAILURE);
        }
        if (-1 == execlp("ls", "ls", NULL)) {
            perror("execve");
            close(fd);
            exit(EXIT_FAILURE);
        }
        close(fd);
        exit(EXIT_SUCCESS);
    } else if (pid[0] == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    pid[1] = fork();
    if (pid[1] == 0) {/* child branch */
        int fd = open(fifopath, O_RDONLY);
        if (fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        /* redirect stdin from pipes read fd */
        if (-1 == dup2(fd, STDIN_FILENO)) {
            perror("dup2");
            close(fd);
            exit(EXIT_FAILURE);
        }
        if (-1 == execlp("wc", "wc", "-l", NULL)) {
            perror("execve");
            close(fd);
            exit(EXIT_FAILURE);
        }
        close(fd);
        exit(EXIT_SUCCESS);
    } else if (pid[1] == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    int rv[2];
    rv[0] = wait_report(pid[0]);
    rv[1] = wait_report(pid[1]);
    
    /* remote fifo */
    if (-1 == unlink(fifopath)) {
        perror("unlink");
    }
    if (rv[0] || rv[1]) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int wait_report(pid_t pid) {
    int status;
    if (-1 == waitpid(pid, &status, 0)) {
        perror("waitpid");
        return -1;
    } else {
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) != 0) {
                fprintf(stderr, "Child processed exited with status %d\n", WEXITSTATUS(status));
                return -1;
            }
        } else if (WIFSIGNALED(status)) {
            fprintf(stderr, "Child processed was terminated by signal %d\n", WTERMSIG(status));
            return -1;
        }
    }
    return 0;    
}
