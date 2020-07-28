#include "../inc/client.h"

void mx_delete_mess(GtkWidget *widget, gpointer data) {
	widget = NULL;
	if (is_editing)
		return;
	t_user_message *current_message = mx_find_row_from_list__row(data);
	// print_history();

	if (current_message) {
		mx_do_message_request(current_message, "delete_message");
	}
}

void mx_delete_message_row(t_user_message *message) {
	if (message && message->row) {
		gtk_widget_destroy(message->row);
		message->row = NULL;
	}

}

gboolean mx_delete_message(t_user_message *new_message) {
	t_user_message *message = NULL;
	int index_msg = mx_get_index_history_message(history_message_list, new_message->owner_id, new_message->tv_id, &message);

    if (message && index_msg != -1) {
    	mx_pop_index(history_message_list, index_msg);
    	mx_delete_message_row(message);
    	mx_messdel(&message);
    }
    return FALSE;
}
