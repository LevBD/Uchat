#include "client.h"

int main(int argc, const char **argv) {
    argc = 0;
    argv = NULL;
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == -1) {
        printf("error = %s\n", strerror(errno));
        return -1;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5000);
    inet_aton("127.0.0.1", &addr.sin_addr);
    if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
        printf("connect error = %s\n", strerror(errno));
        close(sock);
        return -1;
    }
    while (true) {
        json_object *jobj = json_object_new_object();
        json_object *j_type = json_object_new_string("Login");
        json_object *j_login = json_object_new_string("qwerty");
        json_object *j_passwd = json_object_new_string("qwerty");

        json_object_object_add(jobj,"Type", j_type);
        json_object_object_add(jobj,"Login", j_login);
        json_object_object_add(jobj,"Passwd", j_passwd);

        char *test = (char *)json_object_to_json_string(jobj);
        printf("%s\n", test);
        if (write(sock, test, strlen(test)) == -1) {
            printf("error = %s\n", strerror(errno));
            continue;
        }
        int rc = read(sock, test, strlen(test));
        if (rc == 0) {
            printf("Closed connection\n");
            break;
        }
        if (rc == -1)
            printf("error = %s\n", strerror(errno));
        else
            printf("%s\n", test);
    }
    close(sock);
    return 0;
}
