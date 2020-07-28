//LoginLabel

#include "../inc/client.h"

GtkWidget *loginEntry, *passwordEntry;
GtkWidget *statusLabel;
GtkWidget *loginButton;
GtkWidget *loginWindow;
pthread_t loginner;
int logged_in;

char *mx_message_do_login(t_user_info *log_par) {
	char *data = NULL;
	char answ[BUF_SIZE];
    int rc = -1;
	json_object *jobj = json_object_new_object();
	json_object *j_type = json_object_new_string("log_in");
	json_object *j_login = json_object_new_string(log_par->login);
	json_object *j_passwd = json_object_new_string(log_par->password);

	json_object_object_add(jobj, "type", j_type);
	json_object_object_add(jobj, "login", j_login);
	json_object_object_add(jobj, "password", j_passwd);
	data = (char *)json_object_to_json_string(jobj);
	while ((tls_write(tls_ctx, data, strlen(data))) <= 0) {
		rc = mx_do_reconnection(-1);
        if (rc < 0) {
            return LG_ERROR_CONNECTION; 
        }

	}
	if (tls_read(tls_ctx, answ, BUF_SIZE) <= 0) {
		rc = mx_do_reconnection(-1);
        if (rc < 0) {
            return LG_ERROR_CONNECTION; 
        }
	}
	json_object_put(jobj);
	return mx_proc_server_back(answ, log_par);
}

void *mx_login_thread(void *param) {
	// char *res = "incorrect login or password";
	char *res = NULL;
//    char *res = message_connect(((t_user_info *)param)->ip, ((t_user_info *)param)->iport);
	//ippppp port
//	if(!res)
	res = mx_message_do_login((t_user_info *)param);
	if(res)
	{
		gtk_label_set_text(GTK_LABEL(statusLabel), res);
		// message_disconnect();
	}
	else
	{
		mx_init_chat_window(((t_user_info *)param)->nickname);
		logged_in = 1;
		free(param);
		return param;
	}
	free(param);
	gtk_widget_set_sensitive(loginButton, 1);
	return param;
}

void mx_do_login(GtkWidget *widget, gpointer data)
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

	// encrypting login and password:
	char salt[3];
	salt[2] = '\0';
	salt[0] = login[0];
	salt[1] = login[1];
	li->login = strdup(crypt((char *)login, salt)); // free() memory
	salt[0] = password[0];
	salt[1] = password[1];
	li->password = strdup(crypt((char *)password, salt)); // free() memory

	pthread_create(&loginner, 0, mx_login_thread, (void *)li);
}


gboolean mx_check_login(void *param)
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
    mx_init_start_window();
    gtk_widget_hide(loginWindow);
    gtk_widget_show_all(StartWindow);
}

void mx_init_login_window()
{
	GtkBuilder *builder = gtk_builder_new_from_file("client/gld/login.glade");

	loginWindow = GTK_WIDGET(gtk_builder_get_object(builder,"LoginWin"));
	g_signal_connect(loginWindow,"destroy", G_CALLBACK(open_start_win),NULL);

    gtk_window_set_title(GTK_WINDOW(loginWindow), "Sign In");

    gtk_window_set_position(GTK_WINDOW(loginWindow), GTK_WIN_POS_CENTER);
	loginEntry = GTK_WIDGET(gtk_builder_get_object(builder,"LoginEntry"));
	passwordEntry = GTK_WIDGET(gtk_builder_get_object(builder,"PassEntry"));
	g_signal_connect(G_OBJECT(loginEntry),"activate", G_CALLBACK(mx_do_login),NULL);
	g_signal_connect(G_OBJECT(passwordEntry),"activate", G_CALLBACK(mx_do_login),NULL);
	statusLabel = GTK_WIDGET(gtk_builder_get_object(builder,"StatusLabel"));
	loginButton = GTK_WIDGET(gtk_builder_get_object(builder,"LoginButton"));
	g_signal_connect(G_OBJECT(loginButton),"clicked", G_CALLBACK(mx_do_login),NULL);
	
	GtkCssProvider *cssStyle;
	cssStyle = gtk_css_provider_new();
	gtk_css_provider_load_from_path(cssStyle, "./client/src/style.css", NULL);
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
			                               GTK_STYLE_PROVIDER(cssStyle),
			                               GTK_STYLE_PROVIDER_PRIORITY_USER);

	gtk_widget_set_name(loginWindow, "main_window");
	gtk_widget_set_name(loginButton, "send_button");

	logged_in = 0;
	g_timeout_add(50, mx_check_login, 0);
}
