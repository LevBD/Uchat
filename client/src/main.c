#include "../inc/client.h"

t_client_info *get_client_info(void) {
    static t_client_info socket;

    return &socket;
}

static int init_connection(int argc,char *argv[], int sock) {
    t_client_info *inf_sock = get_client_info();
    struct sockaddr_in addr;

    inf_sock->sock = sock;
    if (sock == -1) {
        printf("ERROR: %s\n", strerror(errno));
        return -1;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5003);
    inet_aton("10.111.3.11", &addr.sin_addr);
    // inet_aton("127.0.0.1", &addr.sin_addr);
    if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
        printf("CONNECT ERROR: %s\n", strerror(errno));
        close(sock);
        return -1;
    }
    return 0;
}

int main(int argc,char *argv[]){
	argc = 0;
    argv = NULL;
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (init_connection(argc, argv, sock) < 0)
        return -1;
    gtk_init(&argc, &argv);
    init_start_window();
    gtk_widget_show_all(StartWindow);
    gtk_main();
    close(sock);
    return 0;
}

