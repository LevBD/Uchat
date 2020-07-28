#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/errno.h>
#include <pthread.h>
#include <netdb.h>
#include <stdbool.h>
#include <sys/un.h>
#include <sys/event.h>
#include <fcntl.h>
#include <sqlite3.h>
#include <err.h>
#include "../../libjson/json.h"
#include "../../libressl-3.2.0/include/tls.h"
#include "../../libressl-3.2.0/include/openssl/evp.h"
#include "../../libressl-3.2.0/include/openssl/aes.h"
#include "../../libressl-3.2.0/include/openssl/sha.h"
#include "../../libressl-3.2.0/include/openssl/conf.h"

#define COLS_IN_USERS 4
#define COLS_IN_MESSAGES 4
#define HISTORY_DEPTH 100
#define USERS_LIMIT 256
#define BUFFER_SIZE 2048

struct sockaddr_in addr;
int connected_users[USERS_LIMIT];
sqlite3 *db;
//================== TLS struct declaration ===================
struct tls_config *tls_cfg;
struct tls *tls_ctx;
struct tls *tls_cctx[USERS_LIMIT];

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

typedef bool (*msg_handler)(sqlite3**, t_message);

// ==================== server's main ==========================
void mx_daemonizer();
void mx_kqueue_runner(int server);
void mx_socket_handler(int client_sock);
void mx_sign_in(json_object *jobj, int fd_from);
void mx_sign_up(json_object *jobj, int fd_from);
void mx_parse_message(json_object *jobj, int fd_from);
void mx_get_message_history(int fd_from);
void mx_write_to_socket(char *json_str, int fd_where, int mode);
t_message *mx_json_unpack_message(json_object *jobj);
char *mx_json_pack_message(t_message *msg, int err_code, char *msg_type);
void mx_message_handler(json_object *jobj, int fd_from, char *msg_type, 
                        bool (*msg_handler)(sqlite3**, t_message));

// ================== database API ============================
void mx_db_open(sqlite3 **db, const char *db_name);
void mx_db_init(sqlite3 **db);
int mx_db_verify_user(sqlite3 **db, t_user user);
char *mx_db_get_user_nickname(sqlite3 **db, int user_id);
int mx_db_add_user(sqlite3 **db, t_user new_user);
bool mx_db_add_message(sqlite3 **db, t_message new_message);
bool mx_db_delete_message(sqlite3 **db, t_message del_message);
bool mx_db_update_message(sqlite3 **db, t_message change_message);
t_message *mx_db_get_history(sqlite3 **db, int depth, int *fact_depth);
void mx_clear_history(t_message **history, int fact_depth);
int mx_db_check_login_nickname(sqlite3 **db, t_user user);

#endif
