#include "server.h"

void db_open(sqlite3 **db, const char *db_name) {
	if (sqlite3_open(db_name, db) != SQLITE_OK) {
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(*db));

		exit(1);
	}
}

void db_add_table(sqlite3 **db, const char *sql_stmt) {
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

void init_users(sqlite3 **db) {
	const char *sql_stmt = "CREATE TABLE IF NOT EXISTS users ("
			   "user_id INTEGER NOT NULL PRIMARY KEY,"
			   "user_login TEXT NOT NULL UNIQUE,"
			   "user_password TEXT NOT NULL,"
			   "user_nickname TEXT NOT NULL UNIQUE"
			   ");";
	
	db_add_table(db, sql_stmt);
}

void init_messages(sqlite3 **db) {
	const char *sql_stmt = "CREATE TABLE IF NOT EXISTS messages ("
			   "user_id INTEGER NOT NULL,"
			   "user_nickname TEXT NOT NULL,"
			   "msg_time INTEGER NOT NULL,"
			   "msg_body TEXT NOT NULL,"
			   "PRIMARY KEY (user_id, msg_time)"
			   ");";
	
	db_add_table(db, sql_stmt);
}

void db_init(sqlite3 **db) {
	init_users(db);
	init_messages(db);
}

int db_verify_user(sqlite3 **db, t_user user) {
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
	//printf("db_verify_user_success\n");
	return id;
}

char *db_get_user_nickname(sqlite3 **db, int user_id) {
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
	//printf("db_get_user_nickname_success\n");
	return nickname;
}

int db_add_user(sqlite3 **db, t_user new_user) {
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
	//printf("db_add_user_success\n");
	return db_verify_user(db, new_user);
}

bool db_add_message(sqlite3 **db, t_message new_message) {
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

bool db_delete_message(sqlite3 **db, t_message del_message) {
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

bool db_change_message(sqlite3 **db, t_message change_message) {
	sqlite3_stmt *res;
	const char *sql_stmt = "UPDATE messages "
						   "SET msg_body = '?1' "
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

void db_print_users(sqlite3 **db) {
	sqlite3_stmt *res;
	const char *sql_stmt = "SELECT t.* FROM users AS t;";

	if (sqlite3_prepare_v2(*db, sql_stmt, -1, &res, 0) != SQLITE_OK) {
		fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(*db));
		exit(1);
	}

	printf("id\tlogin\t\tpassword\tnickname\n");
	while (sqlite3_step(res) == SQLITE_ROW) {
		for (int i = 0; i < COLS_IN_USERS; i++) {
			printf("%s", sqlite3_column_text(res, i));
			if (i < COLS_IN_USERS - 1) {
				printf("\t");
			}
		}
		printf("\n");
	}
	sqlite3_finalize(res);
}

t_message *db_get_history(sqlite3 **db, int depth, int *fact_depth) {
	int i = 0;
	t_message *history = (t_message *)malloc(sizeof(t_message) * depth);
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

void clear_history(t_message **history, int fact_depth) {
	t_message *temp = *history;
	for (int i = 0; i < fact_depth; i++) {
		free((void *)temp[i].user_nickname);
		free((void *)temp[i].msg_body);
	}
	free(temp);
	*history = NULL;
}

int db_check_login_nickname(sqlite3 **db, t_user user) {
	sqlite3_stmt *res;
	const char *sql_stmt = "SELECT user_id FROM users WHERE user_login = ?1;";

	if (sqlite3_prepare_v2(*db, sql_stmt, -1, &res, 0) != SQLITE_OK) {
		fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(*db));
		exit(3);
	}
	sqlite3_bind_text(res, 1, user.user_login, -1, SQLITE_STATIC);
	if (sqlite3_step(res) == SQLITE_ROW) {
		sqlite3_finalize(res);
		return -1;
	}
	sqlite3_finalize(res);

	sql_stmt = "SELECT user_id FROM users WHERE user_nickname = ?1;";
	if (sqlite3_prepare_v2(*db, sql_stmt, -1, &res, 0) != SQLITE_OK) {
		fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(*db));
		exit(3);
	}
	sqlite3_bind_text(res, 1, user.user_nickname, -1, SQLITE_STATIC);
	if (sqlite3_step(res) == SQLITE_ROW) {
		sqlite3_finalize(res);
		return -2;
	}
	sqlite3_finalize(res);
	//printf("check_login_nickname_success\n");
	return 0;
}

