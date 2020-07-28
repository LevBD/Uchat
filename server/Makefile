NAME	=	uchat_server

CFLG	=	-std=c11 -Wall -Wextra -Werror -Wpedantic

SRCD	=	src
INCD	=	inc
OBJD	=	obj


INC		=	server.h
INCS	=	$(addprefix $(INCD)/, $(INC))

SRC		=	main.c \
			mx_daemonizer.c \
			mx_soket_handler.c \
			mx_message_handler.c \
			mx_parse_message.c \
			mx_sign_in.c \
			mx_sign_up.c \
			mx_write_to_socket.c \
			mx_kqueue_runner.c \
			mx_get_message_history.c \
			mx_json_pack_message.c \
			mx_json_unpack_message.c \
			mx_db_open.c \
			mx_db_init.c \
			mx_db_verify_user.c \
			mx_db_get_user_nickname.c \
			mx_db_add_user.c \
			mx_db_check_login_nickname.c \
			mx_db_add_message.c \
			mx_db_delete_message.c \
			mx_db_update_message.c \
			mx_db_get_history.c \
			mx_clear_history.c \
			mx_db_api.c


SRCS	=	$(addprefix $(SRCD)/, $(SRC))
OBJS	=	$(addprefix $(OBJD)/, $(SRC:%.c=%.o))

JSONA = ../libjson/libjsonc.a
JSOND = ../libjson
JSONI = ../libjson/inc

LIBRESSL_A = ../libressl-3.2.0/tls/.libs/libtls.a \
			 ../libressl-3.2.0/ssl/.libs/libssl.a \
			 ../libressl-3.2.0/crypto/.libs/libcrypto.a

LIBRESSL_H = \
			-I ../libressl/include/tls.h \
			-I ../libressl/include/openssl \
			-I ../libressl/include/pqueue.h \
			-I ../libressl/tls \
			-I ../libressl/ssl \
			-I ../libressl/crypto


all: $(NAME)

$(FILE:a/%=%)

$(NAME): $(JSONA) $(OBJS)
	@clang $(CFLG) $(OBJS) -lmx -L$(JSOND) -lsqlite3 -o $@ $(JSONA) $(LIBRESSL_H) $(LIBRESSL_A)
	@printf "\r\33[2K$@ \033[32;1mcreated\033[0m\n"

$(OBJD)/%.o: $(SRCD)/%.c $(INCS)
	@clang $(CFLG) -c $< -o $@ -I$(INCD) -I$(JSONI)
	@printf "\r\33[2K$(NAME) \033[33;1mcompile \033[0m$(<:$(SRCD)/%.c=%) "

$(OBJS): | $(OBJD)

$(OBJD):
	@mkdir -p $@

$(JSOND): $(JSONA)

$(JSONA):
	@make -sC $(JSOND)

clean:
	@make -sC $(JSOND) $@
	@rm -rf $(OBJD)
	@printf "$(OBJD)\t   \033[31;1mdeleted\033[0m\n"

uninstall: clean
	@make -sC $(JSOND) $@
	@rm -rf $(NAME)
	@printf "$(NAME) \033[31;1muninstalled\033[0m\n"

reinstall: uninstall all
