#include "server.h"

static void tls_set_config(struct tls_config **tls_cfg, struct tls **tls_ctx);
static void init_database(const char *db_name, sqlite3 **db);
static void mx_bind(int server);
static void mx_listen(int server);
static int init_server_socket(int port);
static void mx_destroyer();

int server = 0;

int main(int argc, const char **argv) {
    tls_cfg = NULL;
	tls_ctx = NULL;

    signal(SIGINT, mx_destroyer);
    signal(SIGKILL, mx_destroyer);
    if (argc != 2) {
        fprintf(stderr, "usage: ./uchat_server port\n");
        return -1;
    }
    memset(connected_users, 0, USERS_LIMIT*sizeof(int));
    mx_daemonizer();
    server = init_server_socket(atoi(argv[1]));
    tls_set_config(&tls_cfg, &tls_ctx);
    mx_bind(server);
    mx_listen(server);
    init_database("uchat.db", &db);
    mx_kqueue_runner(server);
    mx_destroyer(server);
    
    // system("leaks uchat_server");
    return 0;
}

static void tls_set_config(struct tls_config **tls_cfg, struct tls **tls_ctx) {
    if (tls_init() == -1)
        errx(1, "unable to initialize TLS");
    if ((*tls_cfg = tls_config_new()) == NULL)
        errx(1, "unable to allocate TLS config");
    if (tls_config_set_dheparams(*tls_cfg, "auto") == -1)
        errx(1,"unable to set dheparams");
    if (tls_config_set_ca_file(*tls_cfg, "./rcirtificate/root.pem") == -1)
        errx(1, "unable to set root CA file root.pem");
    if (tls_config_set_cert_file(*tls_cfg, "./server.pem") == -1)
        errx(1, "unable to set TLS certificate file server.pem");
    if (tls_config_set_key_file(*tls_cfg, "./server.key") == -1)
        errx(1, "unable to set TLS key file server.key");
    if ((*tls_ctx = tls_server()) == NULL)
        errx(1, "tls server creation failed");
    if (tls_configure(*tls_ctx, *tls_cfg) == -1)
        errx(1, "tls configuration failed (%s)", tls_error(*tls_ctx));
}

static void init_database(const char *db_name, sqlite3 **db) {
    mx_db_open(db, db_name);
    mx_db_init(db);
}

static void mx_bind(int server) {
    int on = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (bind(server, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
        fprintf(stderr, "bind error = %s\n", strerror(errno));
        exit(-1);
    }
}

static void mx_listen(int server) {
    if (listen(server, USERS_LIMIT) == -1) {
        fprintf(stderr, "listen error = %s\n", strerror(errno));
        exit(-1);
    }
}

static int init_server_socket(int port) {
    int server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == -1) {
        fprintf(stderr, "error = %s\n", strerror(errno));
        exit(-1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    // inet_aton("10.111.3.11", &addr.sin_addr);
    // inet_aton("127.0.0.1", &addr.sin_addr);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    return server;
}

static void mx_destroyer() {
    sqlite3_close(db);
    tls_close(tls_ctx);
    tls_free(tls_ctx);
    tls_config_free(tls_cfg);
    close(server);
    exit(0);
}
