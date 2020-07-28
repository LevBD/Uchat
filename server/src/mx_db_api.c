#include "server.h"

// void db_print_users(sqlite3 **db) {
// 	sqlite3_stmt *res;
// 	const char *sql_stmt = "SELECT t.* FROM users AS t;";

// 	if (sqlite3_prepare_v2(*db, sql_stmt, -1, &res, 0) != SQLITE_OK) {
// 		fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(*db));
// 		exit(1);
// 	}

// 	printf("id\tlogin\t\tpassword\tnickname\n");
// 	while (sqlite3_step(res) == SQLITE_ROW) {
// 		for (int i = 0; i < COLS_IN_USERS; i++) {
// 			printf("%s", sqlite3_column_text(res, i));
// 			if (i < COLS_IN_USERS - 1) {
// 				printf("\t");
// 			}
// 		}
// 		printf("\n");
// 	}
// 	sqlite3_finalize(res);
// }

// int db_get_user_last_message_time(sqlite3 **db, int user_id) {
// 	int m_time = 0;
// 	sqlite3_stmt *res;
// 	const char *sql_stmt = "SELECT msg_time FROM messages "
// 	                       "WHERE user_id = ?1 "
// 	                       "ORDER BY msg_time DESC "
// 	                       "LIMIT 1;";

// 	if (sqlite3_prepare_v2(*db, sql_stmt, -1, &res, 0) != SQLITE_OK) {
// 		fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(*db));
// 		exit(3);
// 	}
// 	sqlite3_bind_int(res, 1, user_id);
// 	if (sqlite3_step(res) == SQLITE_ROW) {
// 		m_time = sqlite3_column_int(res, 0);
// 	}
// 	sqlite3_finalize(res);
// 	return m_time;
// }
