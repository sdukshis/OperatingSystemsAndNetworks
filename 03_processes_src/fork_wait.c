#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
  pid_t pid = getpid();
  printf("parent pid: %d\n", pid);

  pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  }
  if (pid) {
    printf("child pid: %d\n", pid);
  }
  else {
    printf("child says this pid: %d\n", getpid());
    printf("child says this parent pid: %d\n", getppid());
    exit(EXIT_SUCCESS);
  }

  int status = 0;
  pid = wait(&status);
  printf("process %d finished with exit code: %d\n", pid, status);
  pid = wait(&status);
  if (pid == -1) {
    perror("wait");
    exit(EXIT_FAILURE);
  }
  return 0;
}