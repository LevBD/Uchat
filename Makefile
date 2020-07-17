NAME = uchat
SERVER_NAME = uchat_server
SERVER_DIR = server
CLIENT_DIR = client
LIBMX_DIR = libmx
JSON_DIR = libjson
SERVER_DB = uchat.db



MAKE_M = make -sf Makefile -C
RM = /bin/rm -rf

all:
	@$(MAKE_M) $(SERVER_DIR) $@
	@$(MAKE_M) $(CLIENT_DIR) $@
	@cp $(addprefix $(CLIENT_DIR)/, $(NAME)) $(addprefix $(SERVER_DIR)/, $(SERVER_NAME)) .

$(NAME):
	@$(MAKE_M) $(CLIENT_DIR)

$(SERVER_NAME):
	@$(MAKE_M) $(SERVER_DIR)

clean:
	@$(MAKE_M) $(SERVER_DIR) $@
	@$(MAKE_M) $(CLIENT_DIR) $@
	@$(MAKE_M) $(LIBMX_DIR) $@
	@$(MAKE_M) $(JSON_DIR) $@

uninstall:
	@$(MAKE_M) $(SERVER_DIR) $@
	@$(MAKE_M) $(CLIENT_DIR) $@
	@$(MAKE_M) $(LIBMX_DIR) $@
	@$(MAKE_M) $(JSON_DIR) $@
	@rm -rf $(NAME) $(SERVER_NAME) $(SERVER_DB) $(addprefix $(SERVER_DIR)/, $(SERVER_DB))

reinstall: uninstall all

.PHONY: all clean uninstall reinstall $(NAME) $(SERVER_NAME)