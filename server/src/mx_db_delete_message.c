#include "server.h"

bool mx_db_delete_message(sqlite3 **db, t_message del_message) {
	sqlite3_stmt *res;
	const char *sql_stmt = "DELETE FROM messages "
	                       "WHERE user_id = ?1 AND msg_time = ?2;";

	if (sqlite3_prepare_v2(*db, sql_stmt, -1, &res, 0) != SQLITE_OK) {
		fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(*db));
		return false;
	}

	sqlite3_bind_int(res, 1, del_message.user_id);
	sqlite3_bind_int(res, 2, del_message.msg_time);

	if (sqlite3_step(res) != SQLITE_DONE) {
		fprintf(stderr, "Error deleting data: %s\n", sqlite3_errmsg(*db));
		return false;
	}
	sqlite3_finalize(res);
	return true;
}
