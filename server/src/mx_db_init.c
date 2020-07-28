#include "server.h"

static void init_users(sqlite3 **db);
static void init_messages(sqlite3 **db);
static void db_add_table(sqlite3 **db, const char *sql_stmt);

void mx_db_init(sqlite3 **db) {
	init_users(db);
	init_messages(db);
}

static void init_users(sqlite3 **db) {
	const char *sql_stmt = "CREATE TABLE IF NOT EXISTS users ("
			   "user_id INTEGER NOT NULL PRIMARY KEY,"
			   "user_login TEXT NOT NULL UNIQUE,"
			   "user_password TEXT NOT NULL,"
			   "user_nickname TEXT NOT NULL UNIQUE"
			   ");";

	db_add_table(db, sql_stmt);
}

static void init_messages(sqlite3 **db) {
	const char *sql_stmt = "CREATE TABLE IF NOT EXISTS messages ("
			   "user_id INTEGER NOT NULL,"
			   "user_nickname TEXT NOT NULL,"
			   "msg_time INTEGER NOT NULL,"
			   "msg_body TEXT NOT NULL,"
			   "PRIMARY KEY (user_id, msg_time)"
			   ");";

	db_add_table(db, sql_stmt);
}

static void db_add_table(sqlite3 **db, const char *sql_stmt) {
	sqlite3_stmt *res;

	if (sqlite3_prepare_v2(*db, sql_stmt, -1, &res, 0) != SQLITE_OK) {
		fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(*db));
		sqlite3_close(*db);
		exit(1);
	}
	if (sqlite3_step(res) != SQLITE_DONE) {
    	fprintf(stderr, "Error creating table: %s\n", sqlite3_errmsg(*db));
    	sqlite3_close(*db);
		exit(1);
	}
	sqlite3_finalize(res);
}
