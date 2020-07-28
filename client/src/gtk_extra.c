#include "../inc/client.h"

gboolean mx_autoscroll(){
    gtk_adjustment_set_value(vAdjust, gtk_adjustment_get_upper(vAdjust));
    return TRUE;
}

void mx_change_title(char *name) {
	if(name == NULL){
		gtk_window_set_title(GTK_WINDOW(chatWindow), "Connection...");
	}
	else {
		gtk_window_set_title(GTK_WINDOW(chatWindow), name);
	}
}

int mx_show_popup(GtkWidget *widget, GdkEvent *event) {
	if (event->type == GDK_BUTTON_PRESS) {

		GdkEvent *bevent = (GdkEvent *) event;

		gtk_menu_popup_at_pointer(GTK_MENU(widget), bevent);

		return TRUE;
	}

	return FALSE;
}

void mx_change_status(char *temp) {
    gtk_label_set_text(GTK_LABEL(statusLabel), temp);
}

gboolean mx_grab_focus(GtkWidget *temp) {
    gtk_widget_grab_focus(temp);
    return FALSE;
}
