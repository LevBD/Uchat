#include "server.h"

void mx_write_to_socket(char *json_str, int fd_where, int mode) {
    int msg_length = strlen(json_str);

    if (mode == 1) {
        for (int i = 3; i < USERS_LIMIT; i++) {
            if (connected_users[i] != 0) {
                tls_write(tls_cctx[i], json_str, msg_length);
            }
        }
    }
    else {
        tls_write(tls_cctx[fd_where], json_str, msg_length);
    }
}
