#include "server.h"

int mx_db_add_user(sqlite3 **db, t_user new_user) {
	sqlite3_stmt *res;
	const char *sql_stmt = "INSERT INTO "
	                       "users (user_login, user_password, user_nickname) "
	                       "VALUES (?1, ?2, ?3);";

	if (sqlite3_prepare_v2(*db, sql_stmt, -1, &res, 0) != SQLITE_OK) {
		fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(*db));
		return -1;
	}

	sqlite3_bind_text(res, 1, new_user.user_login, -1, SQLITE_STATIC);
	sqlite3_bind_text(res, 2, new_user.user_password, -1, SQLITE_STATIC);
	sqlite3_bind_text(res, 3, new_user.user_nickname, -1, SQLITE_STATIC);

	if (sqlite3_step(res) != SQLITE_DONE) {
		fprintf(stderr, "Error inserting data: %s\n", sqlite3_errmsg(*db));
		return -1;
	}
	sqlite3_finalize(res);

	return mx_db_verify_user(db, new_user);
}
