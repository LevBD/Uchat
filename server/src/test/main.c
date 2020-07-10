#include "server.h"

int main(int argc, const char **argv) {

//    if (mx_daemon(argv[1]) == -1) {
//        printf("error 1 = %s\n", strerror(errno));
//        return -1;
//    }
    int server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == -1) {
        printf("error = %s\n", strerror(errno));
        return -1;
    }
//    printf("%d fd server\n", server);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5000);
    inet_aton("127.0.0.1", &addr.sin_addr);

    int on = 1;
    setsockopt(server, SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR), &on, sizeof(on));
    if (bind(server, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
        printf("bind error = %s\n", strerror(errno));
        return -1;
    }
    if (listen(server, 128) == -1) {
        printf("listen error = %s\n", strerror(errno));
        return -1;
    }
    int kq = kqueue();
    if (kq == -1) {
        printf("error = %s\n", strerror(errno));
        close(server);
        return -1;
    }
    struct kevent new_ev;
    EV_SET(&new_ev, server, EVFILT_READ, EV_ADD, 0, 0, 0);
    if (kevent(kq, &new_ev, 1, 0, 0, NULL) == -1) {
        printf("error = %s\n", strerror(errno));
        close(server);
        return -1;
    }
//    struct timespec timeout;
//    timeout.tv_sec = 2;
//    timeout.tv_nsec = 0;
    while(1) {
        int nfds = kevent(kq, NULL, 0, &new_ev, 1, NULL);
        if (nfds == 0) {
//            printf("Timeout\n");
            continue;
        }
        if (nfds == -1) {
            printf("error = %s\n", strerror(errno));
            break;
        }
        if (new_ev.ident == (unsigned long)server) {
            int client_sock = accept(server, NULL, NULL);
            if (client_sock == -1) {
                printf("error = %s\n", strerror(errno));
                break;
            }
//            printf("%d fd client\n", client_sock);
            printf("New client\n");
            EV_SET(&new_ev, client_sock, EVFILT_READ, EV_ADD, 0, 0, 0);
            if (kevent(kq, &new_ev, 1, 0, 0, NULL) == -1) {
                printf("error = %s\n", strerror(errno));
                break;
            }
        }
        else {
//            printf("Interact with fd %d\n", (int)new_ev.ident);
            int rc = mx_socket_handler(new_ev.ident);
//            int rc = interact(new_ev.ident);
            if (rc == -1) {
                printf("error = %s\n", strerror(errno));
                break;
            }
            if ((new_ev.flags & EV_EOF) != 0) {
                printf("Client disconnected\n");
                close(new_ev.ident);
            }
        }
    }
    close(kq);
    close(server);
    return 0;
}
