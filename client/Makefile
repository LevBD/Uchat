NAME	=	uchat

CFLG	=	-std=c11 $(addprefix -W, all extra error pedantic) -g

SRCD	=	src
INCD	=	inc
OBJD	=	obj


INC		=	client.h
INCS	=	$(addprefix $(INCD)/, $(INC))

SRC		=	client.c \


SRCS	=	$(addprefix $(SRCD)/, $(SRC))
OBJS	=	$(addprefix $(OBJD)/, $(SRC:%.c=%.o))

JSONA = ../libjson/libjsonc.a
JSOND = ../libjson
JSONI = ../libjson/inc
all: $(NAME)

$(FILE:a/%=%)

$(NAME): $(JSONA) $(OBJS)
	@clang $(CFLG) $(OBJS) -lmx -L$(JSOND) -o $@ $(JSONA)
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