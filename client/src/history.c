#include "../inc/client.h"

void mx_message_request_history(void) {
	char answ[BUF_SIZE];
	json_object *jobj;

	mx_do_history_request();
	mx_zero_string(answ); /////////// не обнулять а через рид
	tls_read(tls_ctx, answ, BUF_SIZE);
	if (strcmp("no_history", answ) == 0)
		return;
	jobj = json_tokener_parse(answ);
	int hst_size = mx_history_size(jobj);
	mx_create_hst_message_list(hst_size);
	json_object_put(jobj);
}

void mx_do_history_request() {
	const char *data;
	json_object *jobj = json_object_new_object();
	json_object *j_type = json_object_new_string("history_request");
	json_object_object_add(jobj, "type", j_type);

	data = json_object_to_json_string(jobj);
	tls_write(tls_ctx, data, strlen(data));
}

t_list *mx_create_hst_message_list(int list_size) {
	char buffer[BUF_SIZE];
	json_object *jobj;
	const char *type = NULL;
	t_user_message *message = NULL;
	short last = -1;

	for (int i = 0; i < list_size; ++i, message = NULL) {
		mx_do_history_ready();
		last = tls_read(tls_ctx, buffer, BUF_SIZE);
		if (last > 0) {
			buffer[last] = '\0';
			jobj = json_tokener_parse(buffer);
		}
		else
			continue;
		type = json_object_get_string(json_object_object_get(jobj, "type"));
		if (strcmp(type, "hst_list") == 0) {
			message = mx_proc_message_back(jobj);
			if (message) {
				mx_push_back(history_message_list, message);
				mx_add_message_widget(message);
			}
		}
		json_object_put(jobj);

	}
	return history_message_list;
}

void mx_do_history_ready() {
	t_client_info *fd = mx_get_client_info();
	//tls_write(tls_ctx, "r", 1);
	write(fd->socket, "r", 1);
}

int mx_history_size(json_object *jobj) {
	const char *type = json_object_get_string(json_object_object_get(jobj, "type"));

	if (strcmp(type, "hst_size") == 0) {
		return json_object_get_int(json_object_object_get(jobj, "size"));
	}
	mx_strdel((char **)&type);
	return 0;
}
