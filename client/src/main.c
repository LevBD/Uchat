#include "../inc/client.h"

t_client_info *mx_get_client_info(void) {
    static t_client_info argv;

    return &argv;
}



void mx_print_usage(int num) {

    switch (num) {
        case 0:
            printf("usage: ./uchat [ip] [port (> 1024)]\n");
            break;
        case 1: {
            printf("usage: ./uchat");
            printf("\033[1;91m [ip]\033[0m");
            printf(" [port (> 1024)]\n");
            break;
        }
        case 2: {
            printf("usage: ./uchat [ip]");
            printf("\033[1;91m [port (> 1024)]\n\033[0m");
            break;
        }
    }
    exit(1);
}


bool mx_check_ip(char *argv[], char **d_ip) {
    char *ip = strdup(argv[1]);

    if (mx_count_words(ip, '.') != 4) {
    	*d_ip = NULL;
    	return false;
    }
    *d_ip = ip;
    return true;
}

bool mx_check_port(char *argv[], short *port) {
	*port = atoi(argv[2]);

    if (*port <= 1024)
    	return false;
    return true;
}

t_start *mx_check_input(int argc, char *argv[]){
    short port = 0;
    char *ip = NULL;
    t_start *start = NULL;

    if (argc < 3) {
        mx_print_usage(0);
    }
    if (!mx_check_ip(argv, &ip)) {
    	mx_print_usage(1);
    }
    if (!mx_check_port(argv, &port)) {
    	mx_print_usage(2);
    }
    start = (t_start *)malloc(sizeof(t_start));
    start->ip = ip;
    start->port = port;
    return start;
}

int main(int argc,char *argv[]){
    t_client_info *info = mx_get_client_info();
    info->argv = argv;
    owner.last_server_back = NULL;
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    info->socket = sock;
    start_data = mx_check_input(argc, argv);
    if (mx_init_connection(sock, start_data) < 0)
        return -1;
    gtk_init(&argc, &argv);
    mx_init_start_window();
    gtk_widget_show_all(StartWindow);
    gtk_main();
    close(sock);
    return 0;
}

