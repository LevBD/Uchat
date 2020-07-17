//LoginLabel

#include "../inc/client.h"

GtkWidget *loginEntry, *passwordEntry, *passwordConfirm, *nicknameEntry;
GtkWidget *regButton;
GtkWidget *signUpWindow;
GtkWidget *statusLabel;
pthread_t reginer;
int regged_in;

/*====================== GLOBAL =========================*/

void mx_fill_user(t_user_info *to, t_user_info *from) {
    to->login = from->login;
    to->password = from->password;
    to->confpass = from->confpass;
    to->nickname = from->nickname;
    to->id = from->id;
}

char *mx_proc_log_in_back(json_object *jobj, t_user_info *user) {
    int error = json_object_get_int(json_object_object_get(jobj, "error"));

    if (error == 0){
        user->nickname = json_object_get_string(json_object_object_get(jobj, "nickname"));
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

char *mx_proc_server_back(char *buffer, t_user_info *user) {
    json_object *jobj = json_tokener_parse(buffer);
    const char *type = json_object_get_string(json_object_object_get(jobj, "type"));
    
    if (strcmp(type, "log_in_back") == 0)
        return mx_proc_log_in_back(jobj, user);
    else if (strcmp(type, "sign_up_back") == 0)
        return mx_proc_sign_up_back(jobj);
    else if (strcmp(type, "do_message") == 0)
        mx_proc_message_back(jobj);
    return NULL;
}

/*====================== GLOBAL END =====================*/

char *message_do_sing_up(t_user_info *reg_par) {
    t_client_info *clnt = get_client_info();
    char *data = NULL;
    char answ[1024];

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
        write(0, data, strlen(data));                                               /***************/
        write(0, "\n", strlen("\n"));                                               /***************/
        if (write(clnt->sock, data, strlen(data)) == -1) {
            printf("error = %s\n", strerror(errno));
        }
        read(clnt->sock, answ, 1024);
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

void *sing_up_thread(void *param)
{
 // char *res = "incorrect login or password";
    char *res = NULL;
//    char *res = message_connect(((t_user_info *)param)->ip, ((t_user_info *)param)->iport);
    //ippppp port
    if(!res)
       res = message_do_sing_up((t_user_info *)param);
    if(res)
    {
        gtk_label_set_text(GTK_LABEL(statusLabel), res);
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
    gtk_widget_set_sensitive(regButton, 1);
    return param;
}

void do_reg(GtkWidget *widget, gpointer data)
{
    (void) widget;
    (void) data;
    if(!gtk_widget_get_sensitive(regButton))
        return;
    const gchar *login, *password, *confpass, *nickname;
    login = gtk_entry_get_text(GTK_ENTRY(loginEntry));
    if(!login || !*login)
    {
        gtk_widget_grab_focus(loginEntry);
        return;
    }
    nickname = gtk_entry_get_text(GTK_ENTRY(nicknameEntry));
    if(!nickname || !*nickname)
    {
        gtk_widget_grab_focus(nicknameEntry);
        return;
    }
    password = gtk_entry_get_text(GTK_ENTRY(passwordEntry));
    if(!password || !*password)
    {
        gtk_widget_grab_focus(passwordEntry);
        return;
    }
    confpass = gtk_entry_get_text(GTK_ENTRY(passwordConfirm));
    if(!confpass || !*confpass)
    {
        gtk_widget_grab_focus(passwordConfirm);
        return;
    }
    t_user_info *ri = malloc(sizeof(t_user_info));
    ri->login = (char *)login;
    ri->password = (char *)password;
    ri->confpass = (char *)confpass;
    ri->nickname = (char *)nickname;
    init_login_window();
    gtk_widget_set_sensitive(regButton, 1);
    gtk_widget_hide(signUpWindow);
    gtk_widget_show_all(loginWindow);
    pthread_create(&reginer, 0, sing_up_thread, (void *)ri);
}

static void open_start_win(GtkWidget *widget, gpointer data)
{
    (void) widget;
    (void) data;
    init_start_window();
    gtk_widget_hide(signUpWindow);
    gtk_widget_show_all(StartWindow);
}

void init_sign_up_window()
{
    GtkBuilder *builder = gtk_builder_new_from_resource("/org/gtk/client/sign_up.glade");

    signUpWindow = GTK_WIDGET(gtk_builder_get_object(builder,"reg_window"));
    g_signal_connect(signUpWindow,"destroy", G_CALLBACK(open_start_win),NULL);
    gtk_window_set_title(GTK_WINDOW(signUpWindow), "Sign Up");
    gtk_window_set_position(GTK_WINDOW(signUpWindow), GTK_WIN_POS_CENTER);
    loginEntry = GTK_WIDGET(gtk_builder_get_object(builder,"reg_login_entry"));
    passwordEntry = GTK_WIDGET(gtk_builder_get_object(builder,"reg_pass_entry"));
    passwordConfirm = GTK_WIDGET(gtk_builder_get_object(builder,"confirm_pass_entry"));//
    nicknameEntry = GTK_WIDGET(gtk_builder_get_object(builder,"nickname_entry"));//
    g_signal_connect(G_OBJECT(loginEntry),"activate", G_CALLBACK(do_reg),NULL);
    g_signal_connect(G_OBJECT(passwordEntry),"activate", G_CALLBACK(do_reg),NULL);
    g_signal_connect(G_OBJECT(passwordConfirm),"activate", G_CALLBACK(do_reg),NULL);//
    g_signal_connect(G_OBJECT(nicknameEntry),"activate", G_CALLBACK(do_reg),NULL);//
    regButton = GTK_WIDGET(gtk_builder_get_object(builder,"reg_button"));
    statusLabel = GTK_WIDGET(gtk_builder_get_object(builder,"checking_login_label"));
    g_signal_connect(G_OBJECT(regButton),"clicked", G_CALLBACK(do_reg),NULL);
    
    GtkCssProvider *cssStyle;
    cssStyle = gtk_css_provider_new();

    gtk_css_provider_load_from_path(cssStyle, "./client/src/style.css", NULL);

    mx_css_set(cssStyle, signUpWindow);
    mx_css_set(cssStyle, loginEntry);
    mx_css_set(cssStyle, passwordEntry);
    mx_css_set(cssStyle, nicknameEntry);
    mx_css_set(cssStyle, passwordConfirm);
    mx_css_set(cssStyle, regButton);
    regged_in = 0;
    // g_timeout_add(50, check_login, 0);
}
