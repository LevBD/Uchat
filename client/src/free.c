#include "../inc/client.h"

void mx_messdel(t_user_message **message) {
	if (message && *message) {
		mx_strdel((char**)&((*message)->nickname));
    	mx_strdel(&((*message)->data));
    	free(*message);
    	*message = NULL;
	}
}

void mx_clear_node(t_list_node *node) {
	if (node) {
		mx_messdel((t_user_message **)(&node->data));
	}
	node->data = NULL;
}
