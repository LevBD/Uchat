#include "server.h"

static void send_history_size(int size, int fd);

void mx_get_message_history(int fd_from) {
    int fact_depth = 0;
    t_message *history = mx_db_get_history(&db, HISTORY_DEPTH, &fact_depth);

    if (fact_depth == 0) {
        mx_write_to_socket("no_history", fd_from, 0);
        mx_clear_history(&history, fact_depth);
        return;
    }
    send_history_size(fact_depth, fd_from);
    
    for (int i = 0, cntr = fact_depth; cntr > 0; i++) {
        char buf;

        //if (tls_read(tls_cctx[fd_from], &buf, 1) == 1) {
        if (read(fd_from, &buf, 1) == 1) {
            t_message msg = {history[i].user_id, history[i].user_nickname,
                            history[i].msg_time, history[i].msg_body};
            char *json_str = mx_json_pack_message(&msg, 0, "hst_list");
            mx_write_to_socket(json_str, fd_from, 0);
            cntr--;
            free(json_str);
        }
    }
    mx_clear_history(&history, fact_depth);
}


static void send_history_size(int size, int fd) {
    json_object *jback = json_object_new_object();
    json_object *j_type = json_object_new_string("hst_size");
    json_object *j_size = json_object_new_int(size);
    json_object_object_add(jback, "type", j_type);
    json_object_object_add(jback, "size", j_size);

    char *json_str = (char *)json_object_to_json_string(jback);

    mx_write_to_socket(json_str, fd, 0);

    json_object_put(jback);
}
