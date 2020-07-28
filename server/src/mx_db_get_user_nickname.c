#include "server.h"

char *mx_db_get_user_nickname(sqlite3 **db, int user_id) {
	char *nickname = NULL;
	sqlite3_stmt *res;
	const char *sql_stmt = "SELECT user_nickname "
						   "FROM users "
						   "WHERE user_id = ?1;";

	if (sqlite3_prepare_v2(*db, sql_stmt, -1, &res, 0) != SQLITE_OK) {
		fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(*db));
		return NULL;
	}
	sqlite3_bind_int(res, 1, user_id);
	if (sqlite3_step(res) == SQLITE_ROW) {
		nickname = strdup((const char*)sqlite3_column_text(res, 0));
	}
	sqlite3_finalize(res);

	return nickname;
}
