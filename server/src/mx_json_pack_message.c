#include "server.h"

char *mx_json_pack_message(t_message *msg, int err_code, char *msg_type) {
    json_object *jback = json_object_new_object();
    json_object *j_type = json_object_new_string(msg_type);
    json_object *j_error = json_object_new_int(err_code);
    json_object *j_id = json_object_new_int(msg->user_id);
    json_object *j_nickname = json_object_new_string(msg->user_nickname);
    json_object *j_time = json_object_new_int(msg->msg_time);
    json_object *j_body = json_object_new_string(msg->msg_body);

    json_object_object_add(jback, "type", j_type);
    json_object_object_add(jback, "error", j_error);
    json_object_object_add(jback, "user_id", j_id);
    json_object_object_add(jback, "user_nickname", j_nickname);
    json_object_object_add(jback, "msg_time", j_time);
    json_object_object_add(jback, "msg_body", j_body);

    char *json_str = strdup(json_object_to_json_string(jback));
    json_object_put(jback);
    
    return json_str;
}
