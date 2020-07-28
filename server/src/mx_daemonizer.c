#include "server.h"

void mx_daemonizer() {
    if (fork() != 0 ) {
        printf("Pid: %d\n", getpid() + 1);
        exit(0);
    }
    umask(0);
    setsid();
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}
