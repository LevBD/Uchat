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

struct sockaddr_in addr;

int mx_daemon(const char *log_file);
int mx_socket_handler(int client_sock);
//int interact(int client_sock);









#endif
