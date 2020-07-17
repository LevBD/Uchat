#include "server.h"

int main(int argc, const char **argv) {
    //sqlite3 *db;
    argc = 0;
    argv = NULL;
    const char *db_name = "uchat.db";
    int connected_users[USERS_LIMIT];

    //memset(connected_users, 0, USERS_LIMIT);
    for (int i = 0; i < USERS_LIMIT; i++)
        connected_users[i] = 0;

//    if (mx_daemon(argv[1]) == -1) {
//        printf("error 1 = %s\n", strerror(errno));
//        return -1;
//    }
    int server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == -1) {
        fprintf(stderr, "error = %s\n", strerror(errno));
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(5003);
    //inet_aton("10.111.3.11", &addr.sin_addr);
    inet_aton("127.0.0.1", &addr.sin_addr);

    int on = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (bind(server, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
        fprintf(stderr, "bind error = %s\n", strerror(errno));
        return -1;
    }
    if (listen(server, 128) == -1) {
        fprintf(stderr, "listen error = %s\n", strerror(errno));
        return -1;
    }
    int kq = kqueue();
    if (kq == -1) {
        fprintf(stderr, "error = %s\n", strerror(errno));
        close(server);
        return -1;
    }
    struct kevent new_event;
    EV_SET(&new_event, server, EVFILT_READ, EV_ADD, 0, 0, 0);
    if (kevent(kq, &new_event, 1, 0, 0, NULL) == -1) {
        fprintf(stderr, "error = %s\n", strerror(errno));
        close(server);
        return -1;
    }

    struct timespec timeout;
    timeout.tv_sec = 1;
    timeout.tv_nsec = 0;
    
    // database init
    db_open(&db, db_name);
    db_init(&db);

    for (;;) {
        int nfds = kevent(kq, NULL, 0, &new_event, 1, NULL);

        if (nfds == -1) {
            fprintf(stderr, "error = %s\n", strerror(errno));
            break;
        }
        if (new_event.ident == (unsigned long)server) {
            int client_sock = accept(server, NULL, NULL);

            if (client_sock == -1) {
                fprintf(stderr, "error = %s\n", strerror(errno));
                break;
            }
            printf("New client, fd=%d\n", client_sock); // DEBUG line
            // printf("users_id connected:"); // DEBUG line
            // for (int i = 3; i < USERS_LIMIT; i++) { // DEBUG line
            //     if (connected_users[i] != 0) { // DEBUG line
            //         printf(" fd[%d]= id[%d]", i, connected_users[i]); // DEBUG line
            //     } // DEBUG line
            // } 
            // printf("\n"); // DEBUG line

            EV_SET(&new_event, client_sock, EVFILT_READ, EV_ADD, 0, 0, 0);
            if (kevent(kq, &new_event, 1, 0, 0, NULL) == -1) {
                fprintf(stderr, "error = %s\n", strerror(errno));
                break;
            }
        } 
        else {
            mx_socket_handler(new_event.ident, connected_users);
            if ((new_event.flags & EV_EOF) != 0) {
                printf("Client disconnected, fd=%lu\n", new_event.ident); // DEBUG line
                connected_users[new_event.ident] = 0;
                //printf("connected_users[%lu], value=%d\n", new_event.ident, connected_users[new_event.ident]);
                close(new_event.ident);
                
            }
        }
    }
    sqlite3_close(db);
    close(kq);
    close(server);

    system("leaks uchat_server");
    return 0;
}
