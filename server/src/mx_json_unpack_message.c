#include "server.h"

t_message *mx_json_unpack_message(json_object *jobj) {
    int user_id = json_object_get_int(json_object_object_get(jobj, "user_id"));
    const char *user_nickname = json_object_get_string(json_object_object_get(jobj, "user_nickname"));
    int msg_time = json_object_get_int(json_object_object_get(jobj, "msg_time"));
    const char *msg_body = json_object_get_string(json_object_object_get(jobj, "msg_body"));

    t_message *new_message = (t_message*)malloc(sizeof(t_message));

    if (!new_message)
        return NULL;

    new_message->user_id = user_id;
    new_message->user_nickname = user_nickname;
    new_message->msg_time = msg_time;
    new_message->msg_body = msg_body;

    return new_message;
}
