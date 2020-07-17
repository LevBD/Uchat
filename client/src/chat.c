#include "../inc/client.h"

#define REQUEST_HISTORY 10

GtkWidget *chatWindow;
GtkWidget *sendEntry, *sendButton, *messageButton1, *messageButton2;
GtkWidget *messageBox1, *messageBox2;
GtkWidget *statusLabel;
GtkWidget *messagesTreeView;
GtkAdjustment *vAdjust;
GtkScrolledWindow *scrolledWindow;
GtkListStore *messagesListStore;
GtkListBox *messageList;
pthread_t watcher;
pthread_t server;


// void *watcher_thread(void *param) {
// 	(void) param;
// 	scrolledWindow = GTK_SCROLLED_WINDOW(gtk_builder_get_object(builder,"scrolledWindow"));
// 	vAdjust = gtk_scrolled_window_get_vadjustment(scrolledWindow);
// 	pthread_create(&watcher, 0, watcher_thread, 0);
// }

bool mx_is_message_by_data (t_list_node *node, int owner_id, int message_id, t_user_message **message) {
	if (((t_user_message *)(node->data))->tv_id == message_id
		&& ((t_user_message *)(node->data))->owner_id == owner_id) {
		if (message)
			*message = (t_user_message *)(node->data);
		return true;
	}
	return false;
}

int mx_get_index_history_message(t_list *list, int owner_id, int message_id, t_user_message **message) {
	t_list_node *node = list ? list->head : NULL;
	int i = 0;
   	for (; node; node = node->next, i++)
   		if (mx_is_message_by_data(node, owner_id, message_id, message))
       		return i;
    return -1;
}

// int mx_get_index_history_message(t_list *list, int user_id, int message_id) {
// 	{
// 		t_list_node *node = list ? list->head : NULL;
// 		int i = 0;

//     	for (; node; node = node->next, i++)
//         	mx_is_message_by_data(node);
// 	}
// }

t_user_message *mx_proc_message_back(json_object *jobj) { // do free data
    int error = json_object_get_int(json_object_object_get(jobj, "error"));
    int user_id = json_object_get_int(json_object_object_get(jobj, "user_id"));
    char *user_nickname = mx_strdup((char *)json_object_get_string(json_object_object_get(jobj, "user_nickname")));
    int time_id = json_object_get_int(json_object_object_get(jobj, "msg_time"));
    char *msg_body = mx_strdup((char *)json_object_get_string(json_object_object_get(jobj, "msg_body")));
    t_user_message *message = (t_user_message *)malloc(sizeof(t_user_message));

    // if (user_id == owner.id) {
    // 	free(message);
    // 	return NULL;
    // }
    printf("\nMESSAGE from %s\t:::\t\" %s \"\n", user_nickname, msg_body);
    message->owner_id = user_id;
    message->data = msg_body;
    message->tv_id = time_id;
    message->nickname = user_nickname;

    return message;
    // switch (error) {
    //     case 1: return SU_ERROR_DATA_LOGIN;
    //     case 2: return SU_ERROR_DATA_NICKN;
    // }

}

void message_new(t_user_message *message) {
	t_client_info *clnt = get_client_info();
	char *data = NULL;
	char answ[BUF_SIZE];

	json_object *jobj = json_object_new_object();
	json_object *j_type = json_object_new_string("new_message");
	json_object *j_mess = json_object_new_string(message->data);
	json_object *j_id = json_object_new_int(message->owner_id);
	json_object *j_nick = json_object_new_string(message->nickname);

	json_object_object_add(jobj, "type", j_type);
	json_object_object_add(jobj, "msg_body", j_mess);
	json_object_object_add(jobj, "user_id", j_id);
	json_object_object_add(jobj, "user_nickname", j_nick);
	data = (char *)json_object_to_json_string(jobj);
	// mx_printstr("CLIENT->SERVER: ", 0);											/***************/
	// write(0, data, strlen(data));                                               	/***************/
	// write(0, "\n", strlen("\n"));                                               	/***************/
	if (write(clnt->sock, data, strlen(data)) == -1) {
		printf("error = %s\n", strerror(errno));
	}
	// read(clnt->sock, answ, BUF_SIZE);

	// mx_printstr("SERVER: ", 0);													/***************/
	// mx_printstr(answ, 0);
	// mx_printstr("\n", 0);
	free(message);
	json_object_put(jobj);
	// system("leaks -q uchat");
	// printf("Aaaaaaaaaaaaaaaaaaa\n");
	// return mx_proc_server_mess(answ, reg_par);
}

void *watcher_thread(void *param)
{
	// (void) param;
	struct timeval tv;
	struct tm *nowtm;
	char *author = NULL, *body = NULL;
	char timebuf[64];
	message_new((t_user_message *)param);
	// message_request_history(REQUEST_HISTORY);
	// while(1)
	// {
	// 	// int k = message_receive(&tv, &author, &body);
	// 	// if(k < 0)
	// 	// {
	// 	//     gtk_label_set_text(GTK_LABEL(statusLabel), "Disconnected, please restart the client");
	// 	//     gtk_widget_set_sensitive(sendButton, 0);
	// 	//     break;
	// 	// }
	// 	// if(k == 0)
	// 	//     continue;
	// 	if(!author)
	// 	{
	// 		gtk_label_set_text(GTK_LABEL(statusLabel), body);
	// 		continue;
	// 	}
	// 	if(tv.tv_sec)
	// 	{
	// 		nowtm = localtime(&tv.tv_sec);
	// 		strftime(timebuf, 64, "[%d.%m.%Y %H:%M:%S]", nowtm);
	// 	}
	// 	else
	// 	{
	// 		*timebuf = 0;
	// 	}
	// 	// add_list_entry(timebuf, author, body, k != 'h');
	// 	free(author);
	// 	free(body);
	// }
	return param;
}

void *read_server_thread(void * par) {
	t_client_info *clnt = get_client_info();
	char *data = NULL;
	char answ[BUF_SIZE];
	bool flag = true;
	int tail = 0;

	while (flag) {
		if ((tail = read(clnt->sock, answ, BUF_SIZE)) == -1) {
			printf("error = %s\n", strerror(errno));
			flag = false;
		}
		answ[tail - 1] = '\0';
		mx_printstr("SERVER THREAD: ", 0);
		mx_printstr(answ, 0);
		mx_printstr("\n", 0);
	}
	return par;
}

void do_send() {
	const gchar *mess_data;

	mess_data = gtk_entry_get_text(GTK_ENTRY(sendEntry));
	if(!mess_data || !*mess_data)
	{
		gtk_widget_grab_focus(sendEntry);
		return;
	}

	t_user_message *message = (t_user_message *)malloc(sizeof(t_user_message)); // memleak possible
	message->owner_id = owner.id;
	message->data = (char *)mess_data;
	message->nickname = owner.nickname;
	message->tv_id = 0;
	pthread_create(&watcher, 0, watcher_thread, (void *)message);
}

static void zero_string(char *arr) {
	for (int i =0; i < BUF_SIZE; i++)
		arr[i] = '\0';
}

int mx_history_size(json_object *jobj) {
	const char *type = json_object_get_string(json_object_object_get(jobj, "type"));

	if (strcmp(type, "hst_size") == 0) {
		printf("JSON SIZE ::: %d\n", json_object_get_int(json_object_object_get(jobj, "size")));
		return json_object_get_int(json_object_object_get(jobj, "size"));
	}
	mx_strdel((char **)&type);
	return 0;
}

void mx_do_history_ready(int fd) {						//*
	write(fd, "r", 1);									//	*	READY
}														//*

t_list *mx_create_hst_message_list(int list_size, int fd) {
	char buffer[BUF_SIZE];
	json_object *jobj;
	const char *type = NULL;

	for (int i = 0; i < list_size; ++i) {
		mx_do_history_ready(fd);
		read(fd, buffer, BUF_SIZE);
		jobj = json_tokener_parse(buffer);

		type = json_object_get_string(json_object_object_get(jobj, "type"));
		if (strcmp(type, "hst_list") == 0){
			mx_push_back(history_message_list, mx_proc_message_back(jobj));
		}
		printf("JSON MESSAGE ::: \t%s\t%s\n", type, (mx_proc_message_back(jobj))->data);
		json_object_put(jobj);
		zero_string(buffer);
	}
	return history_message_list;
}

/* ============== TEST HISTORY ================*/

void static print_node(t_list_node * node) {
	static int i = 0;
	t_user_message * mess = (t_user_message *)(node->data);

	printf("\n=================================================================================================\n");
	printf("%d)\tMESSAGE FROM \"%s\"\t#%lu\tuser id: %d\t:::\t%s\n", i, mess->nickname, mess->tv_id, mess->owner_id, mess->data);
	printf("=================================================================================================\n");
	i++;
}

void static print_history(t_list *list) {
	system("clear");
	printf("\n\t\t\t::: HISTORY LIST :::\n");
	mx_foreach_list(list, print_node);
}


/* ============== TEST HISTORY END ==============*/

void mx_do_history_request(int fd) {
	const char *data;
	json_object *jobj = json_object_new_object();
	json_object *j_type = json_object_new_string("history_request");
	json_object_object_add(jobj, "type", j_type);

	data = json_object_to_json_string(jobj);
	write(fd, data, strlen(data));
	// mx_strdel((char **)&data);
}

void message_request_history(void) {
	t_client_info *clnt = get_client_info();
	char *data = NULL;
	char answ[BUF_SIZE];
	json_object *jobj;

	// printf("*\n");
	mx_do_history_request(clnt->sock);
	// printf("**\n");
	zero_string(answ); /////////// не обнулять а через рид
	// printf("***\n");
	read(clnt->sock, answ, BUF_SIZE);
	if (strcmp("no_history", answ) == 0)
		return;
	// printf("****\n");
	jobj = json_tokener_parse(answ);
	// printf("*****\n");
	int hst_size = mx_history_size(jobj);
	// printf("******\n");
	mx_create_hst_message_list(hst_size, clnt->sock);
	// printf("*******\n");
	json_object_put(jobj);
}

void init_chat_window(char *nickname)
{
	GtkBuilder *builder = gtk_builder_new_from_resource("/org/gtk/client/chat.glade");
	GtkWidget *box1;
	gint h, w;
	history_message_list = mx_create_list();
	chatWindow = GTK_WIDGET(gtk_builder_get_object(builder,"ChatWindow"));
	char buf[100] = "uchat : ";
	strcat(buf, nickname);
	gtk_window_set_title(GTK_WINDOW(chatWindow), buf);
	g_signal_connect(chatWindow,"destroy", G_CALLBACK(gtk_main_quit),NULL);

	gtk_window_set_position(GTK_WINDOW(chatWindow), GTK_WIN_POS_CENTER);
	sendEntry = GTK_WIDGET(gtk_builder_get_object(builder,"SendEntry"));
	sendButton = GTK_WIDGET(gtk_builder_get_object(builder,"SendButton"));
	g_signal_connect(G_OBJECT(sendEntry),"activate", G_CALLBACK(do_send),NULL);
	g_signal_connect(G_OBJECT(sendButton),"clicked", G_CALLBACK(do_send),NULL);
	//    statusLabel = GTK_WIDGET(gtk_builder_get_object(builder,"statusLabel"));
	scrolledWindow = GTK_SCROLLED_WINDOW(gtk_builder_get_object(builder,"ScrolledWindow"));
	vAdjust = gtk_scrolled_window_get_vadjustment(scrolledWindow);
	messageList = GTK_LIST_BOX(gtk_builder_get_object(builder,"MessageListBox"));
	message_request_history();
	printf("я тут\n");
	print_history(history_message_list);



	/**************** TEST ***************/

	// int o_id = 3;
	// int m_id = 1594995841;
	// t_user_message *message = NULL;
	// int indx = mx_get_index_history_message(history_message_list, o_id, m_id, &message);
	// printf("INDEX ::: #%d\t:::\t\" %p \"\n", indx, message);

	/************* TEST END **************/


	box1 = gtk_label_new("ляяяяяя яяяяяя каааакояяя каааакояяякаааакояяякаа");
	gtk_label_set_line_wrap(GTK_LABEL(box1), TRUE);
	messageBox1 = gtk_box_new (FALSE, 0);
	messageButton1 = gtk_button_new();
	gtk_container_add (GTK_CONTAINER (messageButton1), box1);
	gtk_widget_set_name(messageBox1, "inputMess");

	gtk_widget_set_halign (messageBox1, GTK_ALIGN_END);
	gtk_container_add (GTK_CONTAINER (messageBox1), messageButton1);
	gtk_list_box_insert(messageList, messageBox1, 0);
	GtkCssProvider *cssStyle;
	GtkCssProvider *cssStyle2;
	GtkCssProvider *cssStyle3;
	    cssStyle = gtk_css_provider_new();
	    cssStyle2 = gtk_css_provider_new();
	    cssStyle3 = gtk_css_provider_new();

	    gtk_css_provider_load_from_path(cssStyle, "./client/src/style.css", NULL);
	    gtk_css_provider_load_from_path(cssStyle2, "./client/src/input_mess_style.css", NULL);
	    gtk_css_provider_load_from_path(cssStyle3, "./client/src/other_mess_style.css", NULL);

	    mx_css_set(cssStyle, chatWindow);
	    mx_css_set(cssStyle, sendButton);
	    mx_css_set(cssStyle, sendEntry);
	    mx_css_set(cssStyle2, messageButton1);

	messageBox2 = gtk_box_new (FALSE, 0);
	messageButton2 = gtk_button_new_with_label("Шооооо");

	gtk_widget_set_name(messageBox2, "outMess");
	gtk_container_add (GTK_CONTAINER (messageBox2), messageButton2);
	gtk_list_box_insert(messageList, messageBox2, 1);

	    mx_css_set(cssStyle3, messageButton2);
	 // mx_do_history_request(3);
	 pthread_create(&server, 0, read_server_thread, 0);
}

// void init_chat_window(char *nickname)
// {
// 	GtkBuilder *builder = gtk_builder_new_from_resource("/org/gtk/client/chat.glade");

// 	history_message_list = mx_create_list();
// 	chatWindow = GTK_WIDGET(gtk_builder_get_object(builder,"chatWindow"));
// 	char buf[100] = "uchat : ";
// 	strcat(buf, nickname);
// 	gtk_window_set_title(GTK_WINDOW(chatWindow), buf);
// 	g_signal_connect(chatWindow,"destroy", G_CALLBACK(gtk_main_quit),NULL);

	
// 	gtk_window_set_position(GTK_WINDOW(chatWindow), GTK_WIN_POS_CENTER);
// 	sendEntry = GTK_WIDGET(gtk_builder_get_object(builder,"sendEntry"));
// 	sendButton = GTK_WIDGET(gtk_builder_get_object(builder,"sendButton"));
// 	g_signal_connect(G_OBJECT(sendEntry),"activate", G_CALLBACK(do_send),NULL);
// 	g_signal_connect(G_OBJECT(sendButton),"clicked", G_CALLBACK(do_send),NULL);
// //    statusLabel = GTK_WIDGET(gtk_builder_get_object(builder,"statusLabel"));
// //    messagesTreeView = GTK_WIDGET(gtk_builder_get_object(builder,"messagesTreeView"));
// //    messagesListStore = GTK_LIST_STORE(gtk_builder_get_object(builder,"messagesListStore"));
// 	scrolledWindow = GTK_SCROLLED_WINDOW(gtk_builder_get_object(builder,"scrolledWindow"));
// 	vAdjust = gtk_scrolled_window_get_vadjustment(scrolledWindow);
// 	message_request_history();
// 	print_history(history_message_list);
// 	// mx_do_history_request(3);
// 	pthread_create(&server, 0, read_server_thread, 0);
// }
