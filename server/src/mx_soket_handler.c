#include "server.h"

static int mx_count_array(void *arr, int data);
static void mx_sign_in(json_object *jobj, int *connected_users,int fd_from);
static void mx_sign_up(json_object *jobj, int fd_from);
static void mx_parse_message(json_object *jobj, int *connected_users, int fd_from);
static void mx_handle_new_message(json_object *jobj, int *connected_users, int fd_from);
static void mx_get_message_history();
static void write_to_socket(char *json_str, int *connected_users, int fd_where, int mode);

void mx_socket_handler(int client_sock, int *connected_users) {
    char buffer[BUFFER_SIZE];
    json_object *jobj = NULL;
    
    ssize_t res = read(client_sock, buffer, BUFFER_SIZE);
    
    if (res > 0) {
        jobj = json_tokener_parse(buffer);
        mx_parse_message(jobj, connected_users, client_sock);
    }
    //json_object_put(jback);
}

void mx_parse_message(json_object *jobj, int *connected_users, int fd_from) {
    const char *type = json_object_get_string(json_object_object_get(jobj, "type"));
    
    if (strcmp(type, "new_message") == 0) {
        mx_handle_new_message(jobj, connected_users, fd_from);
    }
    else if (strcmp(type, "history_request") == 0) {
        mx_get_message_history(fd_from);
    }
    else if (strcmp(type, "log_in") == 0) {
        mx_sign_in(jobj, connected_users, fd_from);
    } 
    else if (strcmp(type, "sing_up") == 0) {
        mx_sign_up(jobj, fd_from);
    }
}

static void write_to_socket(char *json_str, int *connected_users, int fd_where, int mode) {
    int msg_length = strlen(json_str);

    if (mode == 1) {
        for (int i = 3; i < USERS_LIMIT; i++) {
            if (connected_users[i] != 0) {
                //printf("DEBUG %d\n", i);
                write(i, json_str, msg_length);
            }
        }
    }
    else {
        write(fd_where, json_str, msg_length);
    }
}

static int mx_count_array(void *arr, int data) {
    int counter = 0;

    for (int i = 0; i < USERS_LIMIT; i++) {
        if (((int *)arr)[i] == data)
            counter++;
    }
    return counter;
}

static void mx_sign_up(json_object *jobj, int fd_from) {
    const char *log = json_object_get_string(json_object_object_get(jobj, "login"));
    const char *pass = json_object_get_string(json_object_object_get(jobj, "password"));
    const char *nick = json_object_get_string(json_object_object_get(jobj, "nickname"));
    json_object *jback = json_object_new_object();
    json_object *j_type = json_object_new_string("sign_up_back");
    json_object *j_error = NULL;

    t_user user = {log, pass, nick};
    int check = db_check_login_nickname(&db, user);

    if (check == -1) {
        j_error = json_object_new_int(1); // incorrect login
    } else if (check == -2) {
        j_error = json_object_new_int(2); // incorrect nickname
    } else if (check == 0) {
        db_add_user(&db, user);
        j_error = json_object_new_int(0);
    }

    json_object_object_add(jback, "type", j_type);
    json_object_object_add(jback, "error", j_error);
    
    char *json_str = (char *)json_object_to_json_string(jback);
    write_to_socket(json_str, NULL, fd_from, 0);

    //json_object_put(jback);
}

static void mx_sign_in(json_object *jobj, int *connected_users, int fd_from) {
    const char *log = json_object_get_string(json_object_object_get(jobj, "login"));
    const char *pass = json_object_get_string(json_object_object_get(jobj, "password"));
    json_object *jback = json_object_new_object();
    json_object *j_type = json_object_new_string("log_in_back");
    json_object *j_error = NULL;
    json_object *j_nickname = NULL;
    json_object *j_id = NULL;
    t_user user = {log, pass, NULL};

    int user_id = db_verify_user(&db, user);
    char *user_nickname = db_get_user_nickname(&db, user_id);
    int is_connected = mx_count_array(connected_users, user_id);
    //printf("user_id = %d, mx_count_array = %d\n", user_id, mx_count_array(connected_users, user_id));
    
    if (user_id > 0 && user_nickname != NULL && is_connected == 0) {
        connected_users[fd_from] = user_id;
        //printf("connected_users[%d]=%d\n", fd_from, mx_count_array(connected_users, user_id));
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
    write_to_socket(json_str, NULL, fd_from, 0);
    //json_object_put(jback);
}

static void mx_handle_new_message(json_object *jobj, int *connected_users, int fd_from) {
    int msg_time = time(NULL);
    int user_id = json_object_get_int(json_object_object_get(jobj, "user_id"));
    const char *user_nickname = json_object_get_string(json_object_object_get(jobj, "user_nickname"));
    const char *msg_body = json_object_get_string(json_object_object_get(jobj, "msg_body"));
    json_object *jback = json_object_new_object();
    json_object *j_error = NULL;
    json_object *j_id = json_object_new_int(user_id);
    json_object *j_nickname = json_object_new_string(user_nickname);
    json_object *j_time = json_object_new_int(msg_time);
    json_object *j_body = json_object_new_string(msg_body);
    json_object *j_type = json_object_new_string("new_message_back");
    t_message new_message;

    new_message.user_id = user_id;
    new_message.user_nickname = user_nickname;
    new_message.msg_time = msg_time;
    new_message.msg_body = msg_body;

    bool message_added = db_add_message(&db, new_message);
    if (message_added) {
        j_error = json_object_new_int(0);
    }
    else {
        j_error = json_object_new_int(1);
    }

    json_object_object_add(jback, "type", j_type);
    json_object_object_add(jback, "error", j_error);
    json_object_object_add(jback, "user_id", j_id);
    json_object_object_add(jback, "user_nickname", j_nickname);
    json_object_object_add(jback, "msg_time", j_time);
    json_object_object_add(jback, "msg_body", j_body);
    
    char *json_str = (char *)json_object_to_json_string(jback);

    if (message_added) {
        write_to_socket(json_str, connected_users, fd_from, 1);
    } 
    else {
        write_to_socket(json_str, NULL, fd_from, 0);
    }
    //json_object_put(jback);
}

static void mx_get_message_history(int fd_from) {
    int fact_depth = 0;
    t_message *history = db_get_history(&db, HISTORY_DEPTH, &fact_depth);

    if (fact_depth == 0) {
        write_to_socket("no_history", NULL, fd_from, 0);
        return;
    }

    json_object *jback = json_object_new_object();
    json_object *j_type = json_object_new_string("hst_size");
    json_object *j_size = json_object_new_int(fact_depth);
    json_object_object_add(jback, "type", j_type);
    json_object_object_add(jback, "size", j_size);

    char *json_str = (char *)json_object_to_json_string(jback);
    
    write_to_socket(json_str, NULL, fd_from, 0);

    //json_object_put(jback);
    json_object *jback2 = NULL;
    json_object *j_error = json_object_new_int(0);
    json_object *j_id = NULL;
    json_object *j_nickname = NULL;
    json_object *j_time = NULL;
    json_object *j_body = NULL;
    j_type = json_object_new_string("hst_list");

    for (int i = 0; i < fact_depth; i++) {

        jback2 = json_object_new_object();

        j_id = json_object_new_int(history[i].user_id);
        j_nickname = json_object_new_string(history[i].user_nickname);
        j_time = json_object_new_int(history[i].msg_time);
        j_body = json_object_new_string(history[i].msg_body);
        json_object_object_add(jback2, "type", j_type);
        json_object_object_add(jback2, "error", j_error);
        json_object_object_add(jback2, "user_id", j_id);
        json_object_object_add(jback2, "user_nickname", j_nickname);
        json_object_object_add(jback2, "msg_time", j_time);
        json_object_object_add(jback2, "msg_body", j_body);
        json_str = (char *)json_object_to_json_string(jback2);
        
        printf("DEBUG[%d]\n", i);

        write_to_socket(json_str, NULL, fd_from, 0);
        //json_object_put(jback2);
    }
    clear_history(&history, fact_depth);
    //json_object_put(jback2);
}





