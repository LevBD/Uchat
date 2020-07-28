#include "server.h"

void mx_message_handler(json_object *jobj, int fd_from, char *msg_type, 
                               bool (*msg_handler)(sqlite3**, t_message)) {
    
    t_message *new_message = mx_json_unpack_message(jobj);
    char *json_str = NULL;

    bool message_handled = (new_message->user_id == connected_users[fd_from] 
                            ? msg_handler(&db, *new_message) : false);

    if (message_handled) {
        json_str = mx_json_pack_message(new_message, 0, msg_type);
        //printf("json object = %s\n", json_str); // DEBUG line
        mx_write_to_socket(json_str, fd_from, 1);
    }
    else {
        json_str = mx_json_pack_message(new_message, 1, msg_type);
        //printf("json object = %s\n", json_str); // DEBUG line
        mx_write_to_socket(json_str, fd_from, 0);
    }
    free(json_str);
    free(new_message);
}
