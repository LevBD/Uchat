//LoginLabel

#include "../inc/client.h"

GtkWidget *loginEntry, *passwordEntry;
GtkWidget *statusLabel;
GtkWidget *loginButton;
GtkWidget *loginWindow;
pthread_t loginner;
int logged_in;

char *message_do_login(t_user_info *log_par) {
	t_client_info *clnt = get_client_info();
	char *data = NULL;
	char answ[1024];
	json_object *jobj = json_object_new_object();
	json_object *j_type = json_object_new_string("log_in");
	json_object *j_login = json_object_new_string(log_par->login);
	json_object *j_passwd = json_object_new_string(log_par->password);

	json_object_object_add(jobj, "type", j_type);
	json_object_object_add(jobj, "login", j_login);
	json_object_object_add(jobj, "password", j_passwd);
	data = (char *)json_object_to_json_string(jobj);
	write(0, data, strlen(data));												/***************/
	write(0, "\n", strlen("\n"));												/***************/
	if (write(clnt->sock, data, strlen(data)) == -1) {
		printf("error = %s\n", strerror(errno));
	}
	read(clnt->sock, answ, 1024);
	printf("SERVER ::: %s\n", answ);
	return mx_proc_server_back(answ, log_par);
}

void *login_thread(void *param)
{
	// char *res = "incorrect login or password";
	char *res = NULL;
//    char *res = message_connect(((t_user_info *)param)->ip, ((t_user_info *)param)->iport);
	//ippppp port
//	if(!res)
	res = message_do_login((t_user_info *)param);
	if(res)
	{
		gtk_label_set_text(GTK_LABEL(statusLabel), res);
		// message_disconnect();
	}
	else
	{
		init_chat_window(((t_user_info *)param)->nickname);
		logged_in = 1;
		free(param);
		return param;
	}
	free(param);
	gtk_widget_set_sensitive(loginButton, 1);
	return param;
}

void do_login(GtkWidget *widget, gpointer data)
{
	(void) widget;
	(void) data;
	if(!gtk_widget_get_sensitive(loginButton))
		return;
	const gchar *login, *password;
	login = gtk_entry_get_text(GTK_ENTRY(loginEntry));
	if(!login || !*login)
	{
		gtk_widget_grab_focus(loginEntry);
		return;
	}
	password = gtk_entry_get_text(GTK_ENTRY(passwordEntry));
	if(!password || !*password)
	{
		gtk_widget_grab_focus(passwordEntry);
		return;
	}
	gtk_widget_set_sensitive(loginButton, 0);
	t_user_info *li = malloc(sizeof(t_user_info));
	li->login = (char *)login;
	li->password = (char *)password;
	pthread_create(&loginner, 0, login_thread, (void *)li);
}


gboolean check_login(void *param)
{
	(void) param;
	if(logged_in)
	{
		gtk_widget_hide(loginWindow);
		gtk_widget_show_all(chatWindow);
		logged_in = 0;
		return G_SOURCE_REMOVE;
	}
	return G_SOURCE_CONTINUE;
}

// void mx_css_set(GtkCssProvider *cssProvider, GtkWidget *widget){
//     GtkStyleContext *con = gtk_widget_get_style_context(widget);
//     gtk_style_context_add_provider(con,
//             GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
// }


static void open_start_win(GtkWidget *widget, gpointer data)
{
    (void) widget;
    (void) data;
    init_start_window();
    gtk_widget_hide(loginWindow);
    gtk_widget_show_all(StartWindow);
}

void init_login_window()
{
	GtkBuilder *builder = gtk_builder_new_from_resource("/org/gtk/client/login.glade");

	loginWindow = GTK_WIDGET(gtk_builder_get_object(builder,"LoginWin"));
	g_signal_connect(loginWindow,"destroy", G_CALLBACK(open_start_win),NULL);

    gtk_window_set_title(GTK_WINDOW(loginWindow), "Sign In");

    gtk_window_set_position(GTK_WINDOW(loginWindow), GTK_WIN_POS_CENTER);
	loginEntry = GTK_WIDGET(gtk_builder_get_object(builder,"LoginEntry"));
	passwordEntry = GTK_WIDGET(gtk_builder_get_object(builder,"PassEntry"));
	g_signal_connect(G_OBJECT(loginEntry),"activate", G_CALLBACK(do_login),NULL);
	g_signal_connect(G_OBJECT(passwordEntry),"activate", G_CALLBACK(do_login),NULL);
	statusLabel = GTK_WIDGET(gtk_builder_get_object(builder,"StatusLabel"));
	loginButton = GTK_WIDGET(gtk_builder_get_object(builder,"LoginButton"));
	g_signal_connect(G_OBJECT(loginButton),"clicked", G_CALLBACK(do_login),NULL);
	
	GtkCssProvider *cssStyle;
    cssStyle = gtk_css_provider_new();

    gtk_css_provider_load_from_path(cssStyle, "./client/src/style.css", NULL);

    mx_css_set(cssStyle, loginWindow);
    mx_css_set(cssStyle, loginEntry);
    mx_css_set(cssStyle, passwordEntry);
    mx_css_set(cssStyle, loginButton);

	logged_in = 0;
	g_timeout_add(50, check_login, 0);
}
