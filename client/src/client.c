#include "client.h"

int main(int argc, const char **argv) {
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == -1) {
        printf("error = %s\n", strerror(errno));
        return -1;
    }
    struct addrinfo *addr;
    struct addrinfo filter;
    memset(&filter, 0, sizeof(filter));
    filter.ai_family = AF_INET;
    filter.ai_socktype = SOCK_STREAM;
    int rc = getaddrinfo(argv[1], argv[2], &filter, &addr);
    if (rc != 0) {
        printf("addrinfo error = %s\n", gai_strerror(rc));
        close(sock);
        return -1;
    }
    if (addr == NULL) {
        printf("not found a server\n");
        freeaddrinfo(addr);
        close(sock);
        return -1;
    }
    rc = connect(sock, addr->ai_addr, addr->ai_addrlen);
    freeaddrinfo(addr);
    if (rc != 0) {
        printf("connect error = %s\n", strerror(errno));
        close(sock);
        return -1;
    }
    int number;
    while (scanf("%d", &number) > 0) {
        if (write(sock, &number, sizeof(number)) == -1) {
            printf("error = %s\n", strerror(errno));
            continue;
        }
        printf("Sent %d\n", number);
        number = 0;
        int rc = read(sock, &number, sizeof(number));
        if (rc == 0) {
            printf("Closed connection\n");
            break;
        }
        if (rc == -1)
            printf("error =%s\n", strerror(errno));
        else
            printf("Receivde %d\n", number);
    }
    close(sock);
    return 0;
}
