#include "../inc/client.h"

void *mx_proc_server_back(char *buffer, t_user_info *user) {
    json_object *jobj = json_tokener_parse(buffer);
    const char *type = json_object_get_string(json_object_object_get(jobj, "type"));
    void *back = NULL;

    user->last_server_back = strdup(type);
    if (strcmp(type, "log_in_back") == 0) {
        back = mx_proc_log_in_back(jobj, user);
        return back;
    }
    else if (strcmp(type, "sign_up_back") == 0) {
        back = mx_proc_sign_up_back(jobj);
        json_object_put(jobj);
        return back;
    }
    else if (strcmp(type, "new_message_back") == 0 || strcmp(type, "update_message_back") == 0 || strcmp(type, "delete_message_back") == 0) {
        back = mx_proc_message_back(jobj);
        json_object_put(jobj);
        return back;
    }
    return NULL;
}

t_user_message *mx_proc_message_back(json_object *jobj) { // do free data
    int error = json_object_get_int(json_object_object_get(jobj, "error"));
    int user_id = json_object_get_int(json_object_object_get(jobj, "user_id"));
    char *user_nickname = strdup((char *)json_object_get_string(json_object_object_get(jobj, "user_nickname")));
    int time_id = json_object_get_int(json_object_object_get(jobj, "msg_time"));
    char *msg_body = strdup((char *)json_object_get_string(json_object_object_get(jobj, "msg_body")));
    t_user_message *message = NULL;

    if (error == 0 && msg_body && *msg_body) {
    	message = (t_user_message *)malloc(sizeof(t_user_message));
	    message->owner_id = user_id;
	    message->data = msg_body;
	    message->tv_id = time_id;
	    message->nickname = user_nickname;
	    message->row = NULL;
	}
    return message;
}

void mx_do_message_request(t_user_message *message, const char *request) {
	char *data = NULL;

	if (!message) {
		return;
	}
	json_object *jobj = json_object_new_object();
	json_object *j_type = json_object_new_string(request);
	json_object *j_mess = json_object_new_string(message->data);
	json_object *j_id = json_object_new_int(message->owner_id);
	json_object *j_msg_id = json_object_new_int((int)(message->tv_id));
	json_object *j_nick = json_object_new_string(message->nickname);

	json_object_object_add(jobj, "type", j_type);
	json_object_object_add(jobj, "msg_body", j_mess);
	json_object_object_add(jobj, "msg_time", j_msg_id);
	json_object_object_add(jobj, "user_id", j_id);
	json_object_object_add(jobj, "user_nickname", j_nick);
	data = (char *)json_object_to_json_string(jobj);                                          	/***************/
	if (tls_write(tls_ctx, data, strlen(data)) == -1) {
		printf("error = %s\n", strerror(errno));
	}
}

void *mx_read_server_thread(void *par) {
	char answ[BUF_SIZE];
	int tail = 0;
	t_user_message *message = NULL;

	while (true) {
		if ((tail = tls_read(tls_ctx, answ, BUF_SIZE)) <= 0) {
			gdk_threads_add_idle((GSourceFunc)mx_change_title, NULL);
			tail = mx_do_reconnection(tail);
			if (tail < 0)
				exit(1);
			lock = false;
    		gdk_threads_add_idle((GSourceFunc)mx_listbox_cleaning, NULL);
			for (int i = 0; i < CNCT_AM && mx_message_do_login(&owner); i++);

			gdk_threads_add_idle((GSourceFunc)mx_change_title, (void *)owner.nickname);
			while (lock) {};
			mx_clear_history();
			mx_message_request_history();
			continue;
		}
		answ[tail] = '\0';
		message = mx_proc_server_back(answ, &owner);
		if (!message)
			continue;
		mx_switch_message_back(&owner, message);
		message = NULL;
	}
	return par;
}

void mx_switch_message_back(t_user_info *user, t_user_message *new_message) {
	char *type = user->last_server_back;
	t_user_message *message = NULL;
	int index_msg = 0;
	t_edit_data *edit = (t_edit_data *)malloc(sizeof(t_edit_data));

    if (strcmp(type, "new_message_back") == 0) {
    	mx_push_back(history_message_list, new_message);
    	gdk_threads_add_idle((GSourceFunc)mx_add_message_widget, new_message);
    	// mx_add_message_widget(new_message);
    }
    else if (strcmp(type, "update_message_back") == 0) {
    	index_msg = mx_get_index_history_message(history_message_list, new_message->owner_id, new_message->tv_id, &message);
    	message->data = strdup(new_message->data);
    	edit->message = message;
    	edit->new_message = new_message;
    	edit->index = index_msg;
    	gdk_threads_add_idle((GSourceFunc)mx_edit_message, edit);
    }
    else if (strcmp(type, "delete_message_back") == 0) {
    	gdk_threads_add_idle((GSourceFunc)mx_delete_message, new_message);
    }
}
