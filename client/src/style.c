#include "../inc/client.h"

void mx_css_set(GtkCssProvider *cssProvider, GtkWidget *widget){
    GtkStyleContext *con = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(con,
            GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}
