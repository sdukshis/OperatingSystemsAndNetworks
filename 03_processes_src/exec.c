/* exec ls -l -a command */
#include <unistd.h>

int main()
{
    int retcode = execlp("ls", "ls", "-l", "-a", NULL);
    if (retcode == -1) {
        perror("execlp");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}