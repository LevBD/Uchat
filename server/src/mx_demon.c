#include "server.h"

int mx_daemon(const char *log_file) {
    if (fork() > 0)
        exit(0);
    int fd = open(log_file, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
    if (fd == -1) {
        printf("open error\n");
        return -1;
    }
    int rc = dup2(fd, STDOUT_FILENO);
    close(fd);
    if (rc == -1) {
        printf("dup error\n");
        return -1;
    }
    close(STDIN_FILENO);
    close(STDERR_FILENO);
    return setsid();
}
