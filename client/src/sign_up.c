#include "../inc/client.h"

GtkWidget *loginEntry, *passwordEntry, *passwordConfirm, *nicknameEntry;
GtkWidget *regButton;
GtkWidget *signUpWindow;
GtkWidget *statusLabel;
pthread_t reginer;
int regged_in;

/*====================== GLOBAL =========================*/

char *mx_proc_log_in_back(json_object *jobj, t_user_info *user) {
    int error = json_object_get_int(json_object_object_get(jobj, "error"));

    if (error == 0){
        user->nickname = strdup(json_object_get_string(json_object_object_get(jobj, "nickname")));
        user->id = json_object_get_int(json_object_object_get(jobj, "user_id"));
    }
    else {
        user->nickname = NULL;
        user->id = 0;
    }
    mx_fill_user(&owner, user);
    switch (error) {
        case 1: return LG_ERROR_DATA;
        case 2: return LG_ERROR_ONLINE;
    }
    return NULL;
}

char *mx_proc_sign_up_back(json_object *jobj) {
    int error = json_object_get_int(json_object_object_get(jobj, "error"));

    switch (error) {
        case 1: return SU_ERROR_DATA_LOGIN;
        case 2: return SU_ERROR_DATA_NICKN;
    }
    return NULL;
}


/*====================== GLOBAL END =====================*/

char *mx_message_do_sing_up(t_user_info *reg_par) {
    char *data = NULL;
    char answ[1024];
    int rc = -1;

    if (strcmp(reg_par->password, reg_par->confpass) == 0) {
        json_object *jobj = json_object_new_object();
        json_object *j_type = json_object_new_string("sing_up");
        json_object *j_login = json_object_new_string(reg_par->login);
        json_object *j_passwd = json_object_new_string(reg_par->password);
        json_object *j_nick = json_object_new_string(reg_par->nickname);

        json_object_object_add(jobj, "type", j_type);
        json_object_object_add(jobj, "login", j_login);
        json_object_object_add(jobj, "password", j_passwd);
        json_object_object_add(jobj, "nickname", j_nick);
        data = (char *)json_object_to_json_string(jobj);
        if (tls_write(tls_ctx, data, strlen(data)) <= 0) {
            rc = mx_do_reconnection(-1);
            if (rc < 0)
                exit(0); // должен выйти на главный экран
        }
        tls_read(tls_ctx, answ, 1024);
        return mx_proc_server_back(answ, reg_par);
    }
    return SU_ERROR_CONFIRM_PASS;
}

// void *login_thread(void *param)
// {
// 	// char *res = "incorrect login or password";
//     char *res = NULL;
// //    char *res = message_connect(((t_user_info *)param)->ip, ((t_user_info *)param)->iport);
//     //ippppp port
// //	if(!res)
// //        res = message_do_login(((t_user_info *)param)->login, ((t_user_info *)param)->password);
//     if(res)
//     {
//         gtk_label_set_text(GTK_LABEL(statusLabel), res);
//         // message_disconnect();
//     }
//     else
//     {
//         init_chat_window(((t_user_info *)param)->login);
//         regged_in = 1;
//         free(param);
//         return param;
//     }
//     free(param);
//     gtk_widget_set_sensitive(reg_button, 1);
//     return param;
// }

void *mx_sing_up_thread(void *param)
{
 // char *res = "incorrect login or password";
    char *res = NULL;
//    char *res = message_connect(((t_user_info *)param)->ip, ((t_user_info *)param)->iport);
    //ippppp port
    if(!res)
       res = mx_message_do_sing_up((t_user_info *)param);
    if(res)
    {
        gdk_threads_add_idle((GSourceFunc)mx_change_status, res);
        // gtk_label_set_text(GTK_LABEL(statusLabel), res);
        // message_disconnect();
    }
    else
    {
        // init_chat_window(((t_user_info *)param)->login);
        regged_in = 1;
        free(param);
        return param;
    }
    free(param);
    // gtk_widget_set_sensitive(regButton, 1);
    return param;
}

void mx_do_reg(GtkWidget *widget, gpointer data)
{
    (void) widget;
    (void) data;
    if(!gtk_widget_get_sensitive(regButton))
        return;
    const gchar *login, *password, *confpass, *nickname;
    login = gtk_entry_get_text(GTK_ENTRY(loginEntry));
    if(!login || !*login)
    {
        gdk_threads_add_idle((GSourceFunc)mx_grab_focus, loginEntry);
        // gtk_widget_grab_focus(loginEntry);
        return;
    }
    nickname = gtk_entry_get_text(GTK_ENTRY(nicknameEntry));
    if(!nickname || !*nickname)
    {
        gdk_threads_add_idle((GSourceFunc)mx_grab_focus, nicknameEntry);
        // gtk_widget_grab_focus(nicknameEntry);
        return;
    }
    password = gtk_entry_get_text(GTK_ENTRY(passwordEntry));
    if(!password || !*password)
    {
        gdk_threads_add_idle((GSourceFunc)mx_grab_focus, passwordEntry);
        // gtk_widget_grab_focus(passwordEntry);
        return;
    }
    confpass = gtk_entry_get_text(GTK_ENTRY(passwordConfirm));
    if(!confpass || !*confpass)
    {
        gdk_threads_add_idle((GSourceFunc)mx_grab_focus, passwordConfirm);
        // gtk_widget_grab_focus(passwordConfirm);
        return;
    }
    t_user_info *ri = malloc(sizeof(t_user_info));

    // encrypting login and password:
    char salt[3];
    salt[2] = '\0';
    salt[0] = login[0];
    salt[1] = login[1];
    ri->login = strdup(crypt((char *)login, salt)); // free() memory
    salt[0] = password[0];
    salt[1] = password[1];
    ri->password = strdup(crypt((char *)password, salt)); // free() memory
    ri->confpass = strdup(crypt((char *)confpass, salt)); // free() memory
    ri->nickname = (char *)nickname;


    pthread_create(&reginer, 0, mx_sing_up_thread, (void *)ri);
}

static void open_start_win(GtkWidget *widget, gpointer data)
{
    (void) widget;
    (void) data;
    mx_init_start_window();
    gtk_widget_hide(signUpWindow);
    gtk_widget_show_all(StartWindow);
}

gboolean mx_check_reg(void *param)
{
    (void) param;
    if(regged_in)
    {
        mx_init_login_window();
        gtk_widget_hide(signUpWindow);
        gtk_widget_show_all(loginWindow);
        regged_in = 0;
        return G_SOURCE_REMOVE;
    }
    return G_SOURCE_CONTINUE;
}

void mx_init_sign_up_window()
{
    GtkBuilder *builder = gtk_builder_new_from_file("client/gld/sign_up.glade");

    signUpWindow = GTK_WIDGET(gtk_builder_get_object(builder,"reg_window"));
    g_signal_connect(signUpWindow,"destroy", G_CALLBACK(open_start_win),NULL);
    gtk_window_set_title(GTK_WINDOW(signUpWindow), "Sign Up");
    gtk_window_set_position(GTK_WINDOW(signUpWindow), GTK_WIN_POS_CENTER);
    loginEntry = GTK_WIDGET(gtk_builder_get_object(builder,"reg_login_entry"));
    passwordEntry = GTK_WIDGET(gtk_builder_get_object(builder,"reg_pass_entry"));
    passwordConfirm = GTK_WIDGET(gtk_builder_get_object(builder,"confirm_pass_entry"));
    nicknameEntry = GTK_WIDGET(gtk_builder_get_object(builder,"nickname_entry"));
    g_signal_connect(G_OBJECT(loginEntry),"activate", G_CALLBACK(mx_do_reg), NULL);
    g_signal_connect(G_OBJECT(passwordEntry),"activate", G_CALLBACK(mx_do_reg), NULL);
    g_signal_connect(G_OBJECT(passwordConfirm),"activate", G_CALLBACK(mx_do_reg), NULL);
    g_signal_connect(G_OBJECT(nicknameEntry),"activate", G_CALLBACK(mx_do_reg), NULL);
    regButton = GTK_WIDGET(gtk_builder_get_object(builder,"reg_button"));
    statusLabel = GTK_WIDGET(gtk_builder_get_object(builder,"checking_login_label"));
    g_signal_connect(G_OBJECT(regButton),"clicked", G_CALLBACK(mx_do_reg),NULL);
    
    GtkCssProvider *cssStyle;
    cssStyle = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssStyle, "./client/src/style.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                           GTK_STYLE_PROVIDER(cssStyle),
                                           GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_widget_set_name(signUpWindow, "main_window");
    gtk_widget_set_name(regButton, "send_button");
    
    regged_in = 0;
    g_timeout_add(50, mx_check_reg, 0);
    // g_timeout_add(50, check_login, 0);
}
