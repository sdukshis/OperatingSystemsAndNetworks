/* Copy stdin to stdout until SIGINT received */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t signal_status = 0;  /* special type to safely change in 
                                                signal handlers */

void signal_handler(int signal) {
    signal_status = signal;
}

int main() {
    if (SIG_ERR == signal(SIGINT, signal_handler)) {
        fprintf(stderr, "signal error");
        return EXIT_FAILURE;
    }
    while (signal_status != SIGINT) {
        // Do some usefull work
        sleep(1);
    }
    printf("Gracefully finishing\n");
    return EXIT_SUCCESS;
}
