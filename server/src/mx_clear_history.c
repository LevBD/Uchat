#include "server.h"

void mx_clear_history(t_message **history, int fact_depth) {
	if (*history == NULL) {
		return;
	}
	t_message *temp = *history;
	for (int i = 0; i < fact_depth; i++) {
		free((void *)temp[i].user_nickname);
		free((void *)temp[i].msg_body);
	}
	free(temp);
	*history = NULL;
}
