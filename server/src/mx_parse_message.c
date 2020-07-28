#include "server.h"

void mx_parse_message(json_object *jobj, int fd_from) {
    const char *type = json_object_get_string(json_object_object_get(jobj, "type"));

    if (strcmp(type, "new_message") == 0) {
        json_object *j_time = json_object_new_int(time(NULL));
        json_object_object_add(jobj, "msg_time", j_time);
        mx_message_handler(jobj, fd_from, "new_message_back", mx_db_add_message);
    }
    else if (strcmp(type, "delete_message") == 0) {
        mx_message_handler(jobj, fd_from, "delete_message_back", mx_db_delete_message);
    }
    else if (strcmp(type, "update_message") == 0) {
        mx_message_handler(jobj, fd_from, "update_message_back", mx_db_update_message);
    }
    else if (strcmp(type, "history_request") == 0) {
        mx_get_message_history(fd_from);
    }
    else if (strcmp(type, "log_in") == 0) {
        mx_sign_in(jobj, fd_from);
    }
    else if (strcmp(type, "sing_up") == 0) {
        mx_sign_up(jobj, fd_from);
    }
}
