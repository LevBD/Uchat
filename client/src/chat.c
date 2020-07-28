#include "../inc/client.h"

/* ============== TEST HISTORY ================*/

// void static print_node(t_list_node *node) {
// 	static int i = 0;
// 	t_user_message *mess = (t_user_message *)(node->data);

// 	printf("\n=================================================================================================\n");
// 	printf("%d)\tMESSAGE FROM \"%s\"\t#%lu\tuser id: %d\t:::\t%s\n", i, mess->nickname, mess->tv_id, mess->owner_id, mess->data);
// 	printf("=================================================================================================\n");
// 	i++;
// }

// void print_history() {
// 	system("clear");
// 	printf("\n\t\t\t::: HISTORY LIST :::\n");
// 	mx_foreach_list(history_message_list, print_node);
// }

/* ============ END TEST HISTORY ===============*/


void mx_init_chat_window(char *nickname)
{
	GtkBuilder *builder = gtk_builder_new_from_file("client/gld/chat.glade");

	history_message_list = mx_create_list();
	chatWindow = GTK_WIDGET(gtk_builder_get_object(builder,"ChatWindow"));
	gtk_window_set_title(GTK_WINDOW(chatWindow), nickname);
	g_signal_connect(chatWindow,"destroy", G_CALLBACK(gtk_main_quit),NULL);
	is_editing = false;

	gtk_window_set_position(GTK_WINDOW(chatWindow), GTK_WIN_POS_CENTER);
	sendEntry = GTK_WIDGET(gtk_builder_get_object(builder,"SendEntry"));
	sendButton = GTK_WIDGET(gtk_builder_get_object(builder,"SendButton"));
	hendler_id_entry = g_signal_connect(G_OBJECT(sendEntry),"activate", G_CALLBACK(mx_do_send),NULL);
	hendler_id_button = g_signal_connect(G_OBJECT(sendButton),"clicked", G_CALLBACK(mx_do_send),NULL);
	Viewport = GTK_WIDGET(gtk_builder_get_object(builder,"Viewport"));
	scrolledWindow = GTK_SCROLLED_WINDOW(gtk_builder_get_object(builder,"ScrolledWindow"));
	vAdjust = gtk_scrolled_window_get_vadjustment(scrolledWindow);
	messageList = GTK_LIST_BOX(gtk_builder_get_object(builder,"MessageListBox"));
	
	mx_message_request_history();
	GtkCssProvider *cssStyle;
	cssStyle = gtk_css_provider_new();
	gtk_css_provider_load_from_path(cssStyle, "./client/src/style.css", NULL);
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
			                               GTK_STYLE_PROVIDER(cssStyle),
			                               GTK_STYLE_PROVIDER_PRIORITY_USER);

	gtk_widget_set_name(Viewport, "viewport");
	gtk_widget_set_name(chatWindow, "main_window");
	gtk_widget_set_name(sendButton, "send_button");
	gtk_widget_set_name(sendEntry, "send_entry");
	pthread_create(&server, 0, mx_read_server_thread, NULL);
}
