#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/errno.h>
#include <pthread.h>
#include <netdb.h>
#include <stdbool.h>
#include <sys/un.h>
#include <sys/event.h>
#include <fcntl.h>
#include <sqlite3.h>
#include "../../libjson/json.h"


#define COLS_IN_USERS 4
#define COLS_IN_MESSAGES 4
#define HISTORY_DEPTH 50
#define USERS_LIMIT 256
#define BUFFER_SIZE 2048

sqlite3 *db; // global variable !!!!!!!!

struct sockaddr_in addr;

typedef struct s_user {
	const char *user_login;
	const char *user_password;
	const char *user_nickname;
} t_user;

typedef struct s_message {
	int user_id;
	const char *user_nickname;
	time_t msg_time;
	const char *msg_body;
} t_message;

int mx_daemon(const char *log_file);
void mx_socket_handler(int client_sock, int *connected_users);
//int interact(int client_sock);

void db_open(sqlite3 **db, const char *db_name);
void db_add_table(sqlite3 **db, const char *sql_stmt);
void init_users(sqlite3 **db);
void init_messages(sqlite3 **db);
void db_init(sqlite3 **db);
int db_verify_user(sqlite3 **db, t_user user);
char *db_get_user_nickname(sqlite3 **db, int user_id);
int db_add_user(sqlite3 **db, t_user new_user);
bool db_add_message(sqlite3 **db, t_message new_message);
void db_print_users(sqlite3 **db);
t_message *db_get_history(sqlite3 **db, int depth, int *fact_depth);
void clear_history(t_message **history, int fact_depth);
int db_check_login_nickname(sqlite3 **db, t_user user);
bool db_delete_message(sqlite3 **db, t_message del_message);
bool db_change_message(sqlite3 **db, t_message change_message);

#endif
