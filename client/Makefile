NAME_C = uchat

SRCD = src
INCD = inc
OBJD = obj

LJSOND	= ../libjson
LJSONX  = ../libjson/libjsonc.a
LJSONA := $(addprefix $(LJSOND)/, $(LJSONX))

INCS = uchat.h

SRC_CLIENT = client.c

SRCS_CLIENT = $(addprefix $(SRCD)/client/, $(SRC_CLIENT))

OBJS_CLIENT = $(addprefix $(OBJD)/, $(SRC_CLIENT:%.c=%.o))

CFLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic

all: install

$(OBJD):
	@mkdir -pv $@

client: $(NAME_C)

$(NAME_C): $(OBJS_CLIENT)
	@clang $(CFLAGS) `pkg-config --cflags --libs gtk+-3.0` $(LJSONA) $(OBJS_CLIENT) -o $@
	@printf "\r\33[2K$@\t\t   \033[32;1mcreated\033[0m\n"

$(OBJD)/%.o: src/%.c $(INCS)
	@clang $(CFLAGS) `pkg-config --cflags gtk+-3.0` -o $@ -c $< -I$(INCD)
	@printf "\r\33[2K\033[37;1mcompile \033[0m$(<:$(SRCD)/%.c=%) "

#$(OBJD)/%.o: src/functions/%.c $(INCS)
#	@clang $(CFLAGS) `pkg-config --cflags gtk+-3.0` -o $@ -c $< -I$(INCD) -I$(LMXI)
#	@printf "\r\33[2K\033[37;1mcompile \033[0m$(<:$(SRCD)/%.c=%) "

$(OBJS_CLIENT): | $(OBJD)

install: client

clean:
	@make -sC ./libjson clean
	@rm -rf $(OBJD)
	@printf "$(OBJD)\t\t   \033[31;1mdeleted\033[0m\n"

uninstall: clean
	@make -sC ./libjson uninstall
	@rm -rf $(NAME_C)
	@printf "$(NAME_C)\t\t   \033[31;1muninstalled\033[0m\n"

reinstall: uninstall install
