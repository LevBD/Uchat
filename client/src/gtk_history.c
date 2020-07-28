#include "../inc/client.h"

void mx_listbox_node_cleaning(t_list_node *node){
	t_user_message *temp = ((t_user_message *)(node->data));
	if (temp && temp->row) {
		gtk_widget_destroy(temp->row);
		temp->row = NULL;
	}
}

gboolean mx_listbox_cleaning(){
	lock = true;
	mx_foreach_list(history_message_list, mx_listbox_node_cleaning);
	lock = false;
	return FALSE;
}

GtkWidget *mx_create_out_mess(const char *message_text, const char *login_text, time_t time_v){
	GtkWidget *popup_menu;
	GtkWidget *row;
	GtkWidget *in_row;
	GtkWidget *nickname;
	GtkWidget *time;
	GtkWidget *message_label;
	GtkWidget *message;
	GtkWidget *void_box;
	GtkWidget *container;
	GtkWidget *edit;
	GtkWidget *delete;
	GtkWidget *ebox;

	popup_menu = gtk_menu_new();

	row = gtk_list_box_row_new ();

    gtk_widget_show(row);
	gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(row), FALSE); // !
	gtk_list_box_row_set_selectable(GTK_LIST_BOX_ROW(row), FALSE); // !

	in_row = gtk_box_new (GTK_ORIENTATION_VERTICAL, TRUE);
    gtk_widget_show(in_row);
	nickname = gtk_label_new(login_text);
    gtk_widget_show(nickname);
	gtk_widget_set_halign (nickname, GTK_ALIGN_END);
	message_label = gtk_label_new(message_text);
    gtk_widget_show(message_label);
	gtk_label_set_line_wrap(GTK_LABEL(message_label), TRUE);
	message = gtk_button_new();
    gtk_widget_show(message);
	gtk_container_add (GTK_CONTAINER(message), message_label);
	time = gtk_label_new(mx_short_time(&time_v));
    gtk_widget_show(time);
	void_box = gtk_box_new(FALSE, 0);
    gtk_widget_show(void_box);
	gtk_container_add (GTK_CONTAINER(void_box), time);
	gtk_box_set_baseline_position(GTK_BOX(void_box), GTK_BASELINE_POSITION_CENTER);

	ebox = gtk_event_box_new();
    gtk_widget_show(ebox);

	gtk_container_add (GTK_CONTAINER(ebox), message);
	gtk_container_add (GTK_CONTAINER(in_row), ebox);
	gtk_container_add (GTK_CONTAINER(in_row), nickname);

	container = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, TRUE);
    gtk_widget_show(container);

	gtk_container_add (GTK_CONTAINER(container), void_box);
	gtk_container_add (GTK_CONTAINER(container), in_row);

	edit = gtk_menu_item_new_with_label("Edit");
	gtk_menu_item_activate(GTK_MENU_ITEM(edit));
	gtk_widget_show(edit);
	gtk_menu_shell_append(GTK_MENU_SHELL(popup_menu), edit);

	delete = gtk_menu_item_new_with_label("Delete");
	gtk_menu_item_activate(GTK_MENU_ITEM(delete));
	gtk_widget_show(delete);
	gtk_menu_shell_append(GTK_MENU_SHELL(popup_menu), delete);

	g_signal_connect_swapped(G_OBJECT(ebox), "button-press-event",
	G_CALLBACK(mx_show_popup), popup_menu);

	gtk_container_add (GTK_CONTAINER(row), container);
	gtk_widget_set_halign (row, GTK_ALIGN_END);

	gtk_widget_set_name(message, "output_message");
	gtk_widget_set_name(row, "row");
	gtk_widget_set_name(edit, "edit_tool");
	gtk_widget_set_name(delete, "edit_tool");
	gtk_widget_set_name(popup_menu, "popup_menu");
	gtk_widget_set_name(nickname, "nickname");
	gtk_widget_set_name(time, "time");
	gtk_widget_set_name(message_label, "message");

	g_signal_connect(G_OBJECT(delete),"activate", G_CALLBACK(mx_delete_mess), row);
	g_signal_connect(G_OBJECT(edit),"activate", G_CALLBACK(mx_edit_mess), row);

	return row;
}

GtkWidget *mx_create_in_mess(const char *message_text, const char *login_text, time_t time_v) {
	GtkWidget *row;
	GtkWidget *in_row;
	GtkWidget *nickname;
	GtkWidget *time;
	GtkWidget *message_label;
	GtkWidget *message;
	GtkWidget *void_box;
	GtkWidget *container;

	row = gtk_list_box_row_new ();

    gtk_widget_show(row);
	gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(row), FALSE); // !
	gtk_list_box_row_set_selectable(GTK_LIST_BOX_ROW(row), FALSE); // !

	in_row = gtk_box_new (GTK_ORIENTATION_VERTICAL, TRUE);
    gtk_widget_show(in_row);
	nickname = gtk_label_new(login_text);
    gtk_widget_show(nickname);
	gtk_widget_set_halign (nickname, GTK_ALIGN_START);
	message_label = gtk_label_new(message_text);
    gtk_widget_show(message_label);
	gtk_label_set_line_wrap(GTK_LABEL(message_label), TRUE);
	message = gtk_button_new();
    gtk_widget_show(message);
	gtk_container_add (GTK_CONTAINER(message), message_label);
	time = gtk_label_new(mx_short_time(&time_v));
    gtk_widget_show(time);
	void_box = gtk_box_new(FALSE, 0);
    gtk_widget_show(void_box);
	gtk_container_add (GTK_CONTAINER(void_box), time);
	gtk_box_set_baseline_position(GTK_BOX(void_box), GTK_BASELINE_POSITION_CENTER);

	gtk_container_add (GTK_CONTAINER(in_row), message);
	gtk_container_add (GTK_CONTAINER(in_row), nickname);

	container = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, TRUE);
    gtk_widget_show(container);

	gtk_container_add (GTK_CONTAINER(container), in_row);
	gtk_container_add (GTK_CONTAINER(container), void_box);

	gtk_container_add (GTK_CONTAINER(row), container);
	gtk_widget_set_halign (row, GTK_ALIGN_START);

	gtk_widget_set_name(message, "input_message");
	gtk_widget_set_name(nickname, "nickname");
	gtk_widget_set_name(time, "time");
	gtk_widget_set_name(message_label, "message");

	return row;
}
