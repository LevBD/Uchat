#include "../inc/client.h"

bool mx_is_message_by_data(t_list_node *node, int owner_id, int message_id, t_user_message **message) {
	if (message)
		*message = NULL;

	if (((t_user_message *)(node->data))->tv_id == message_id
		&& ((t_user_message *)(node->data))->owner_id == (unsigned int)owner_id) {
		if (message) {
			*message = (node->data);
		}
		return true;
	}
	return false;
}

char *mx_short_time(time_t *t) {
    char *s;
    if ((time(0) - *t) < 86400 && (time(0) - *t) >= 0)
        s = mx_strndup((ctime)(t) + 11, 5);
    else {
        s = mx_strndup((ctime)(t) + 4, 12);
        // s = mx_strncat(s, ((ctime)(t) + 19), 5);
    }
    return s;
}

void mx_zero_string(char *arr) {
	for (int i =0; i < BUF_SIZE; i++)
		arr[i] = '\0';
}

void *mx_find_row_from_list__row(void *widget) {
	for (t_list_node *node = history_message_list->head; node; node = node->next) {

		if (node && node->data && widget == (void *)(((t_user_message *)(node->data))->row))
			return (t_user_message *)(node->data);
	}
	return NULL;
}

void mx_fill_user(t_user_info *to, t_user_info *from) {
    to->login = from->login;
    to->password = from->password;
    to->confpass = from->confpass;
    to->nickname = from->nickname;
    to->id = from->id;
}
