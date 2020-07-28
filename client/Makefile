NAME	=	uchat

CFLG	=	-std=c11 -Wall -Wextra -Werror -Wpedantic -Wno-unused-command-line-argument

SRCD	=	src/
INCD	=	inc/
OBJD	=	obj/
GLDD	=	gld/

GLD 	= 	start.glade \
			login.glade \
			sign_up.glade \
			chat.glade

GTK = `pkg-config --cflags --libs gtk+-3.0`

SGLD	=	$(GLDD)$(GLD)
SRCS	=	$(wildcard $(SRCD)*.c)
OBJS	=	$(SRCS:$(SRCD)%.c=$(OBJD)%.o)

JSONA = 	../libjson/libjsonc.a
JSOND = 	../libjson
JSONI = 	../libjson/inc

LIBMXA = 	../libmx/libmx.a
LIBMXD = 	../libmx
LIBMXI = 	../libmx/inc

LIBRESSL_A = \
			../libressl-3.2.0/tls/.libs/libtls.a \
			../libressl-3.2.0/ssl/.libs/libssl.a \
			../libressl-3.2.0/crypto/.libs/libcrypto.a

LIBRESSL_H = \
			-I ../libressl/include \
			-I ../libressl/include/openssl \
			-I ../libressl/tls \
			-I ../libressl/ssl \
			-I ../libressl/crypto

IFLG = -I$(INCD) -I$(JSONI) -I$(LIBMXI) $(LIBRESSL_H)

all: install_json install_libmx install

$(OBJD):
	@mkdir -p $@

$(OBJD)%.o: $(SRCD)%.c
	@clang $(CFLG) $(GTK) $(IFLG) -c $< -o $@
	@printf "\r\33[2K$(NAME) \033[33;1mcompile \033[0m$(<:$(SRCD)/%.c=%)"

install: $(OBJD) $(OBJS)
	@clang $(CFLG) $(SRCS) $(JSONA) $(LIBMXA) $(GTK) $(IFLG) $(LIBRESSL_A) -o $(NAME)
	@printf "\r\33[2K$(NAME) \033[32;1mcreated\033[0m\n"

install_libmx:
	@make -sC $(LIBMXD)

install_json:
	@make -sC $(JSOND)

clean:
	@rm -rf $(GLD)
	@make -sC $(JSOND) $@
	@rm -rf $(OBJD)
	@printf "$(OBJD)\t   \033[31;1mdeleted\033[0m\n"

uninstall: clean
	@make -sC $(JSOND) $@
	@rm -rf $(NAME)
	@printf "$(NAME) \033[31;1muninstalled\033[0m\n"

reinstall: uninstall all
