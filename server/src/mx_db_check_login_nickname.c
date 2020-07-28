#include "server.h"

static int db_check_login(sqlite3 **db, t_user *user);
static int db_check_nickname(sqlite3 **db, t_user *user);

int mx_db_check_login_nickname(sqlite3 **db, t_user user) {
	int result = 0;

	if ((result = db_check_login(db, &user)) < 0)
		return result;
	if ((result = db_check_nickname(db, &user)) < 0)
		return result;
	
	return result;
}

static int db_check_login(sqlite3 **db, t_user *user) {
	sqlite3_stmt *res;
	const char *sql_stmt = "SELECT user_id FROM users WHERE user_login = ?1;";

	if (sqlite3_prepare_v2(*db, sql_stmt, -1, &res, 0) != SQLITE_OK) {
		fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(*db));
		exit(3);
	}
	sqlite3_bind_text(res, 1, user->user_login, -1, SQLITE_STATIC);
	if (sqlite3_step(res) == SQLITE_ROW) {
		sqlite3_finalize(res);
		return -1;
	}
	sqlite3_finalize(res);

	return 0;
}

static int db_check_nickname(sqlite3 **db, t_user *user) {
	sqlite3_stmt *res;
	const char *sql_stmt = "SELECT user_id FROM users WHERE user_nickname = ?1;";
	
	if (sqlite3_prepare_v2(*db, sql_stmt, -1, &res, 0) != SQLITE_OK) {
		fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(*db));
		exit(3);
	}
	sqlite3_bind_text(res, 1, user->user_nickname, -1, SQLITE_STATIC);
	if (sqlite3_step(res) == SQLITE_ROW) {
		sqlite3_finalize(res);
		return -2;
	}
	sqlite3_finalize(res);

	return 0;
}
