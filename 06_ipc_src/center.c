/* Prints window size of currentn pseudoterminal */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s text\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char *msg = argv[1];

    /* get window size of current terminal */
    struct winsize ws;
    ioctl(0, TIOCGWINSZ, &ws);

    int msglen = strlen(msg);
    int padding_top = ws.ws_row / 2;
    int padding_bottom = padding_top;
    int padding_left = (ws.ws_col - msglen) / 2;

    for (int i = 0; i < padding_top; ++i) {
        puts("");
    }
    for (int i = 0; i < padding_left; ++i) {
        putchar(' ');
    }

    puts(msg);

    for (int i = 0; i < padding_bottom; ++i) {
        puts("");
    }
    return EXIT_SUCCESS;
}
