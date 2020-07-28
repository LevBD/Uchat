#include "server.h"

void mx_socket_handler(int client_sock) {
    char buffer[BUFFER_SIZE];
    json_object *jobj = NULL;

    ssize_t rc = tls_read(tls_cctx[client_sock], buffer, BUFFER_SIZE);
    if (rc > 0) {
        jobj = json_tokener_parse(buffer);
        if (jobj != NULL) {
            mx_parse_message(jobj, client_sock);
            json_object_put(jobj);
        }
    }
}
