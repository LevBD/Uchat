#include "server.h"

int mx_db_verify_user(sqlite3 **db, t_user user) {
	int id = 0;
	sqlite3_stmt *res;
	const char *sql_stmt = "SELECT user_id "
						   "FROM users "
						   "WHERE user_login = ?1 AND user_password = ?2;";

	if (sqlite3_prepare_v2(*db, sql_stmt, -1, &res, 0) != SQLITE_OK) {
		fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(*db));
		return -1;
	}
	sqlite3_bind_text(res, 1, user.user_login, -1, SQLITE_STATIC);
	sqlite3_bind_text(res, 2, user.user_password, -1, SQLITE_STATIC);
	if (sqlite3_step(res) == SQLITE_ROW) {
		id = sqlite3_column_int(res, 0);
	}
	sqlite3_finalize(res);

	return id;
}
