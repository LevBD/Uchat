#include "server.h"

t_message *mx_db_get_history(sqlite3 **db, int depth, int *fact_depth) {
	int i = 0;

	t_message *history = (t_message *)malloc(sizeof(t_message) * depth);
	if (history == NULL) {
		return NULL;
	}
	sqlite3_stmt *res;
	const char *sql_stmt = "SELECT user_id, user_nickname, msg_time, msg_body "
	                       "FROM messages "
	                       "ORDER BY msg_time ASC "
	                       "LIMIT ?1";

	if (sqlite3_prepare_v2(*db, sql_stmt, -1, &res, 0) != SQLITE_OK) {
		fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(*db));
		return NULL;
	}
	sqlite3_bind_int(res, 1, depth);
   	while (sqlite3_step(res) == SQLITE_ROW) {
   		history[i].user_id = sqlite3_column_int(res, 0);
		history[i].user_nickname = strdup((const char*)sqlite3_column_text(res, 1));
		history[i].msg_time = sqlite3_column_int(res, 2);
		history[i].msg_body = strdup((const char*)sqlite3_column_text(res, 3));
		++i;
	}
	sqlite3_finalize(res);
	*fact_depth = i;
	return history;
}

// t_message *mx_db_get_history(sqlite3 **db, int depth, int *fact_depth, int user_id) {
// 	int i = 0;
// 	int m_time = db_get_user_last_message_time(db, user_id);
// 	t_message *history = (t_message *)malloc(sizeof(t_message) * depth);
// 	if (m_time == 0 || history == NULL) {
// 		return NULL;
// 	}
// 	sqlite3_stmt *res;
// 	const char *sql_stmt = "SELECT user_id, user_nickname, msg_time, msg_body "
// 	                       "FROM messages "
// 	                       "WHERE msg_time > ?1 "
// 	                       "ORDER BY msg_time ASC "
// 	                       "LIMIT ?2";

// 	if (sqlite3_prepare_v2(*db, sql_stmt, -1, &res, 0) != SQLITE_OK) {
// 		fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(*db));
// 		return NULL;
// 	}
// 	sqlite3_bind_int(res, 1, m_time);
// 	sqlite3_bind_int(res, 2, depth);
// 	sqlite3_bind_int(res, 1, depth);
//    	while (sqlite3_step(res) == SQLITE_ROW) {
//    		history[i].user_id = sqlite3_column_int(res, 0);
// 		history[i].user_nickname = strdup((const char*)sqlite3_column_text(res, 1));
// 		history[i].msg_time = sqlite3_column_int(res, 2);
// 		history[i].msg_body = strdup((const char*)sqlite3_column_text(res, 3));
// 		++i;
// 	}
// 	sqlite3_finalize(res);
// 	*fact_depth = i;
// 	return history;
// }
