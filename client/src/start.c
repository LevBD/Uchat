//LoginLabel

#include "../inc/client.h"


GtkWidget *RegButton;
GtkWidget *SignInButton;
GtkWidget *StartWindow;

pthread_t loginner;

// void *login_thread(void *param)
// {
//     init_login_window();
//     free(param);
//     gtk_widget_set_sensitive(SignInButton, 1);
//     return param;
// }

// void *reg_thread(void *param)
// {
//     init_login_window();
//     free(param);
//     gtk_widget_set_sensitive(SignInButton, 1);
//     return param;
// }

void open_login_win(GtkWidget *widget, gpointer data)
{
    (void) widget;
    (void) data;
    init_login_window();
    gtk_widget_set_sensitive(SignInButton, 1);
    gtk_widget_hide(StartWindow);
    gtk_widget_show_all(loginWindow);
    // pthread_create(&loginner, 0, login_thread);
}

void open_reg_win(GtkWidget *widget, gpointer data)
{
    (void) widget;
    (void) data;
    init_sign_up_window();
    gtk_widget_set_sensitive(RegButton, 1);
    gtk_widget_hide(StartWindow);
    gtk_widget_show_all(signUpWindow);
    // pthread_create(&loginner, 0, reg_thread);
}

// void mx_css_set(GtkCssProvider *cssProvider, GtkWidget *widget){
//     GtkStyleContext *con = gtk_widget_get_style_context(widget);
//     gtk_style_context_add_provider(con,
//             GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
// }

void init_start_window()
{
    GtkBuilder *builder = gtk_builder_new_from_resource("/org/gtk/client/start.glade");

    StartWindow = GTK_WIDGET(gtk_builder_get_object(builder,"start_window"));

    g_signal_connect(StartWindow,"destroy", G_CALLBACK(gtk_main_quit),NULL);
    
    gtk_window_set_position(GTK_WINDOW(StartWindow), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(StartWindow), "Uchat");
    RegButton = GTK_WIDGET(gtk_builder_get_object(builder,"OpenSignUpWin"));
    g_signal_connect(G_OBJECT(RegButton),"clicked", G_CALLBACK(open_reg_win),NULL);
    SignInButton = GTK_WIDGET(gtk_builder_get_object(builder,"OpenSignInWin"));
    g_signal_connect(G_OBJECT(SignInButton),"clicked", G_CALLBACK(open_login_win),NULL);

    GtkCssProvider *cssStyle;
    cssStyle = gtk_css_provider_new();

    gtk_css_provider_load_from_path(cssStyle, "./client/src/style.css", NULL);

    mx_css_set(cssStyle, StartWindow);
    mx_css_set(cssStyle, RegButton);
    mx_css_set(cssStyle, SignInButton);

}