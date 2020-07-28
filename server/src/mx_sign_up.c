#include "server.h"

void mx_sign_up(json_object *jobj, int fd_from) {
    const char *log = json_object_get_string(json_object_object_get(jobj, "login"));
    const char *pass = json_object_get_string(json_object_object_get(jobj, "password"));
    const char *nick = json_object_get_string(json_object_object_get(jobj, "nickname"));
    json_object *jback = json_object_new_object();
    json_object *j_type = json_object_new_string("sign_up_back");
    json_object *j_error = NULL;

    t_user user = {log, pass, nick};
    int check = mx_db_check_login_nickname(&db, user);

    if (check == -1) {
        j_error = json_object_new_int(1); // incorrect login
    } else if (check == -2) {
        j_error = json_object_new_int(2); // incorrect nickname
    } else if (check == 0) {
        mx_db_add_user(&db, user);
        j_error = json_object_new_int(0);
    }

    json_object_object_add(jback, "type", j_type);
    json_object_object_add(jback, "error", j_error);

    char *json_str = (char *)json_object_to_json_string(jback);
    //printf("json object = %s\n", json_str);
    mx_write_to_socket(json_str, fd_from, 0);
}
