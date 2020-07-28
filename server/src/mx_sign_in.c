#include "server.h"

static int count_array(void *arr, int data);

void mx_sign_in(json_object *jobj, int fd_from) {
    const char *log = json_object_get_string(json_object_object_get(jobj, "login"));
    const char *pass = json_object_get_string(json_object_object_get(jobj, "password"));
    json_object *jback = json_object_new_object();
    json_object *j_type = json_object_new_string("log_in_back");
    json_object *j_error = NULL;
    json_object *j_nickname = NULL;
    json_object *j_id = NULL;
    t_user user = {log, pass, NULL};

    int user_id = mx_db_verify_user(&db, user);
    char *user_nickname = mx_db_get_user_nickname(&db, user_id);
    int is_connected = count_array(connected_users, user_id);

    if (user_id > 0 && user_nickname != NULL && is_connected == 0) {
        connected_users[fd_from] = user_id;
        j_error = json_object_new_int(0);
        j_nickname = json_object_new_string(user_nickname);
        j_id = json_object_new_int(user_id);
    } else if (user_id <= 0) {
        j_error = json_object_new_int(1);
        j_nickname = json_object_new_string("");
    } else if (is_connected != 0) {
        j_error = json_object_new_int(2);
        j_nickname = json_object_new_string("");
    }
    json_object_object_add(jback, "type", j_type);
    json_object_object_add(jback, "error", j_error);
    json_object_object_add(jback, "nickname", j_nickname);
    json_object_object_add(jback, "user_id", j_id);

    char *json_str = (char *)json_object_to_json_string(jback);
    //printf("json object = %s\n", json_str);
    mx_write_to_socket(json_str, fd_from, 0);
}

static int count_array(void *arr, int data) {
    int counter = 0;

    for (int i = 0; i < USERS_LIMIT; i++) {
        if (((int *)arr)[i] == data)
            counter++;
    }
    return counter;
}
