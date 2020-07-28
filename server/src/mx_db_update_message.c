#include "server.h"

bool mx_db_update_message(sqlite3 **db, t_message change_message) {
	sqlite3_stmt *res;
	const char *sql_stmt = "UPDATE messages "
						   "SET msg_body = ?1 "
	                       "WHERE user_id = ?2 AND msg_time = ?3;";

	if (sqlite3_prepare_v2(*db, sql_stmt, -1, &res, 0) != SQLITE_OK) {
		fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(*db));
		return false;
	}

	sqlite3_bind_text(res, 1, change_message.msg_body, -1, SQLITE_STATIC);
	sqlite3_bind_int(res, 2, change_message.user_id);
	sqlite3_bind_int(res, 3, change_message.msg_time);

	if (sqlite3_step(res) != SQLITE_DONE) {
		fprintf(stderr, "Error updating data: %s\n", sqlite3_errmsg(*db));
		return false;
	}
	sqlite3_finalize(res);
	return true;
}
