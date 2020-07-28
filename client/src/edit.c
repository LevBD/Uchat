#include "../inc/client.h"

void mx_edit_message_complite(GtkWidget *widget, gpointer data) {
	(void) widget;
	char *new_mess = (char *)gtk_entry_get_text(GTK_ENTRY(sendEntry));
	t_user_message *temp = mx_create_edit_message((t_user_message *)data, new_mess);
	if (temp) {
		mx_do_message_request(temp, "update_message");
		is_editing = false;
	}
}

t_user_message *mx_create_edit_message(t_user_message *message, char *msg_body) {
	t_user_message *new = NULL;

	if (message) {
		new = (t_user_message *)malloc(sizeof(t_user_message));
		new->owner_id = message->owner_id;
		new->data = strdup(msg_body);
		new->tv_id = message->tv_id;
		new->nickname = message->nickname;
	}
	return new;
}

void mx_edit_mess(GtkWidget *widget, gpointer data) {
	is_editing = true;
	t_user_message *current_message = mx_find_row_from_list__row(data);

	widget = NULL;
	gtk_entry_set_text(GTK_ENTRY(sendEntry), current_message->data);
	g_signal_handler_disconnect(sendEntry, hendler_id_entry);
	g_signal_handler_disconnect(sendButton, hendler_id_button);
	hendler_id_entry = g_signal_connect(G_OBJECT(sendEntry),"activate", G_CALLBACK(mx_edit_message_complite), current_message);
	hendler_id_button = g_signal_connect(G_OBJECT(sendButton),"clicked", G_CALLBACK(mx_edit_message_complite), current_message);
}

gboolean mx_edit_message(t_edit_data *edit) {
	if (edit->message && edit->message->row && edit->new_message){

    	mx_delete_message_row(edit->message);
		if (edit->message->owner_id == owner.id)
			edit->message->row = mx_create_out_mess(edit->new_message->data, edit->new_message->nickname, edit->new_message->tv_id);
		else
			edit->message->row = mx_create_in_mess(edit->new_message->data, edit->new_message->nickname, edit->new_message->tv_id);
		gtk_widget_show(edit->message->row);
		gtk_list_box_insert(messageList, edit->message->row, edit->index);

		gtk_entry_set_text(GTK_ENTRY(sendEntry), "");
		g_signal_handler_disconnect(sendEntry, hendler_id_entry);
		g_signal_handler_disconnect(sendButton, hendler_id_button);
		hendler_id_entry = g_signal_connect(G_OBJECT(sendEntry),"activate", G_CALLBACK(mx_do_send), NULL);
		hendler_id_button = g_signal_connect(G_OBJECT(sendButton),"clicked", G_CALLBACK(mx_do_send), NULL);
	}
	return FALSE;
}
