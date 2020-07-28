#include "server.h"

void mx_db_open(sqlite3 **db, const char *db_name) {
	if (sqlite3_open(db_name, db) != SQLITE_OK) {
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(*db));

		exit(1);
	}
}
