#include "server.h"

bool mx_db_add_message(sqlite3 **db, t_message new_message) {
	sqlite3_stmt *res;
	const char *sql_stmt = "INSERT INTO "
	                       "messages (user_id, user_nickname, msg_time, msg_body) "
	                       "VALUES (?1, ?2, ?3, ?4);";

	if (sqlite3_prepare_v2(*db, sql_stmt, -1, &res, 0) != SQLITE_OK) {
		fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(*db));
		return false;
	}

	sqlite3_bind_int(res, 1, new_message.user_id);
	sqlite3_bind_text(res, 2, new_message.user_nickname, -1, SQLITE_STATIC);
	sqlite3_bind_int(res, 3, new_message.msg_time);
	sqlite3_bind_text(res, 4, new_message.msg_body, -1, SQLITE_STATIC);

	if (sqlite3_step(res) != SQLITE_DONE) {
		fprintf(stderr, "Error inserting data: %s\n", sqlite3_errmsg(*db));
		return false;
	}
	sqlite3_finalize(res);
	return true;
}
