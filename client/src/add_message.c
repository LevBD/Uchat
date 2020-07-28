#include "../inc/client.h"

gboolean mx_add_message_widget(t_user_message *new_message) {
	GtkWidget *temp = NULL;

	if (!new_message)
		return FALSE;
	if(new_message->owner_id == owner.id){
    	temp = mx_create_out_mess(new_message->data, new_message->nickname, new_message->tv_id);
    }
    else {
    	temp = mx_create_in_mess(new_message->data, new_message->nickname, new_message->tv_id);
    }
    new_message->row = temp;
    gtk_widget_show(temp);
    gtk_list_box_insert(messageList, temp, -1);
    return FALSE;
}

void mx_do_send() {
	char *mess_data;

	mess_data = strdup(gtk_entry_get_text(GTK_ENTRY(sendEntry)));
	if(!mess_data || !*mess_data)
	{
		gtk_widget_grab_focus(sendEntry);
		return;
	}

	t_user_message *message = (t_user_message *)malloc(sizeof(t_user_message)); // memleak possible
	message->owner_id = owner.id;
	message->data = mess_data;
	message->nickname = owner.nickname;
	message->tv_id = 0;
	pthread_create(&watcher, 0, mx_watcher_thread, (void *)message);
	gtk_entry_set_text(GTK_ENTRY(sendEntry), "");
	gdk_threads_add_idle((GSourceFunc)mx_autoscroll, NULL);
}

void *mx_watcher_thread(void *param)
{
	mx_do_message_request((t_user_message *)param, "new_message");
	return param;
}
