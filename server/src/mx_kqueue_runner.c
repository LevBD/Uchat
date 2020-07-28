#include "server.h"

static int init_kqueue(int server);
static int get_kqueue_event(int kq, struct kevent *new_event, int fd, int server);
static int accept_new_client(int server);
static void handle_client_event(struct kevent *new_event);

void mx_kqueue_runner(int server) {
    struct timespec timeout;
    timeout.tv_sec = 1;
    timeout.tv_nsec = 0;

    int kq = init_kqueue(server);
    struct kevent new_event;

    if (get_kqueue_event(kq, &new_event, server, server) == -1)
        exit(-1);
    
    for (;;) {
        if (kevent(kq, NULL, 0, &new_event, 1, NULL) == -1) {
            fprintf(stderr, "error = %s\n", strerror(errno));
            //break;
        }

        if (new_event.ident == (unsigned long)server) {
            int client_sock = accept_new_client(server);
            
            if (client_sock != -1) {
                if (get_kqueue_event(kq, &new_event, client_sock, server) == -1)
                    break;
            }
        } 
        else
            handle_client_event(&new_event);
    }
    close(kq);
}

static int init_kqueue(int server) {
    int kq = kqueue();
    if (kq == -1) {
        fprintf(stderr, "kqueue init error = %s\n", strerror(errno));
        close(server);
        exit(-1);
    }
    return kq;
}

static int get_kqueue_event(int kq, struct kevent *new_event, int fd, int server) {
    EV_SET(new_event, fd, EVFILT_READ, EV_ADD, 0, 0, 0);
    if (kevent(kq, new_event, 1, 0, 0, NULL) == -1) {
        fprintf(stderr, "kevent error = %s\n", strerror(errno));
        if (fd == server)
            close(fd);
        return -1;
    }
    return 0;
}

static int accept_new_client(int server) {
    int client_sock = accept(server, NULL, NULL);
    int i = 0;

    if (client_sock == -1) {
        fprintf(stderr, "accept error = %s\n", strerror(errno));
        return -1;
    }
    if (client_sock >= USERS_LIMIT) {
        fprintf(stderr, "clients limit exceeded = %s\n", strerror(errno));
        return -1;
    }
    if (tls_accept_socket(tls_ctx, &tls_cctx[client_sock], client_sock) == -1)
        fprintf(stderr, "tls accept failed (%s)\n", tls_error(tls_cctx[client_sock]));

    if((i = tls_handshake(tls_cctx[client_sock])) == -1)
        fprintf(stderr, "tls handshake failed (%s)\n", tls_error(tls_cctx[client_sock]));
    printf("New client, fd=%d\n", client_sock); // DEBUG line
    return client_sock;
}

static void handle_client_event(struct kevent *new_event) {
    mx_socket_handler(new_event->ident);
    
    if ((new_event->flags & EV_EOF) != 0) {
        printf("Client disconnected, fd=%lu\n", new_event->ident); // DEBUG line
        connected_users[new_event->ident] = 0;
        tls_free(tls_cctx[new_event->ident]);
        tls_close(tls_cctx[new_event->ident]);
        close(new_event->ident);
    }
}
