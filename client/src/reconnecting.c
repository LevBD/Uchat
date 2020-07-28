#include "../inc/client.h"

int mx_init_connection(int sock, t_start *start_data) {
    struct sockaddr_in addr;
    tls_cfg = NULL;
    tls_ctx = NULL;

    if (sock == -1) {
        printf("ERROR: %s\n", strerror(errno));
        return -1;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(start_data->port);
    addr.sin_addr.s_addr = inet_addr(start_data->ip);
    inet_aton(start_data->ip, &addr.sin_addr);
    if (tls_init() == -1)
        errx(1, "unable to initialize TLS");
    if ((tls_cfg = tls_config_new()) == NULL)
        errx(1, "unable to allocate TLS config");
    tls_config_insecure_noverifyname(tls_cfg);
    if (tls_config_set_dheparams(tls_cfg, "legacy") == -1)
        errx(1,"unable to set dheparams");
    if (tls_config_set_ca_file(tls_cfg, "./rcirtificate/root.pem") == -1)
        errx(1, "unable to set root CA file root.pem");
    if (tls_config_set_cert_file(tls_cfg, "client.pem") == -1)
        errx(1, "unable to set TLS certificate file client.pem");
    if (tls_config_set_key_file(tls_cfg, "client.key") == -1)
        errx(1, "unable to set TLS key file client.key");

    if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
        printf("CONNECT ERROR: %s\n", strerror(errno));
        close(sock);
        return -1;
    }
    if ((tls_ctx = tls_client()) == NULL)
        errx(1, "tls client creation failed");
    // tls_config_insecure_noverifyname(tls_cfg);
    if (tls_configure(tls_ctx, tls_cfg) == -1)
        errx(1, "tls configuration failed (%s)",
            tls_error(tls_ctx));
    if (tls_connect_socket(tls_ctx, sock, "name") == -1) {
        errx(1, "tls connection failed (%s)",
            tls_error(tls_ctx));
    }
    int i = 0;
    if ((i = tls_handshake(tls_ctx)) == -1)
            errx(1, "tls handshake failed (%s)", tls_error(tls_ctx));
    // mx_report_tls(tls_ctx, "uchat");
    return 0;
}

int mx_do_reconnection(int rc) {
	t_client_info *info = mx_get_client_info();
	int counter = 0;

	while(rc <= 0) {
		sleep(CNCT_CLDN);
		printf("TRY CONNECTING\n"); // выод на панели
		close(info->socket);
		if (tls_ctx){
			tls_close(tls_ctx);
			tls_free(tls_ctx);
			tls_ctx = NULL;
		}
		info->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (mx_init_connection(info->socket, start_data) == 0)
			return 0;
		if (counter++ == CNCT_AM)
			return -1;
	}
	return 0;
}
