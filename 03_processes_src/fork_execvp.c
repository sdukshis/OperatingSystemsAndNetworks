/* exec ls -l -a command */
#include <unistd.h>

int main()
{
    char *args[] = {"ls", "-l", "-a", NULL};
    int retcode = execvp(args[0], args);
    if (retcode == -1) {
        perror("execlp");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}