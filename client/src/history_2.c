#include "../inc/client.h"

void mx_clear_history(void) {
	// mx_delete_list(history_message_list);
	// history_message_list = mx_create_list();
	mx_foreach_list(history_message_list, mx_clear_node);
	while (history_message_list->head && history_message_list->tail)
		mx_pop_back(history_message_list);
}

int mx_get_index_history_message(t_list *list, int owner_id, int message_id, t_user_message **message) {
	t_list_node *node = list ? list->head : NULL;
	int i = 0;

   	for (; node; node = node->next, i++)
   		if (mx_is_message_by_data(node, owner_id, message_id, message))
       		return i;
    return -1;
}
