NAME	=	uchat

CFLG	=	-std=c11 $(addprefix -W, all extra error pedantic) -g

SRCD	=	src
INCD	=	inc
OBJD	=	obj
GLDD	=	gld


INC		=	client.h
INCS	=	$(addprefix $(INCD)/, $(INC))

RS		=	resources.c \
			style.c

SRC		=	start.c\
			sign_up.c\
			login.c\
			chat.c\
			main.c


GLD 	= 	start.glade\
			login.glade\
			sign_up.glade\
			chat.glade\

SGLD	=	$(addprefix $(GLDD)/, $(GLD))

SRCS	=	$(addprefix $(SRCD)/, $(SRC))
RSS	=	$(addprefix $(SRCD)/, $(RS))

OBJS	=	$(addprefix $(OBJD)/, $(SRC:%.c=%.o))

JSONA = 	../libjson/libjsonc.a
JSOND = 	../libjson
JSONI = 	../libjson/inc

LIBMXA = 	../libmx/libmx.a
LIBMXD = 	../libmx
LIBMXI = 	../libmx/inc


all: install_json install_libmx install clean

install:
	@cp $(SGLD) ./
	@glib-compile-resources ./src/client.gresource.xml --target=./src/resources.c --generate-source
	@clang $(SRCS) $(RSS) $(JSONA) $(LIBMXA) `pkg-config --cflags --libs gtk+-3.0` -o $(NAME) -I$(INCS) -I$(INCD) -I$(JSONI) -I$(LIBMXI)
	@printf "\r\33[2K$(NAME) \033[32;1mcreated\033[0m\n"

install_libmx:
	@make -sC $(LIBMXD)

install_json:
	@make -sC $(JSOND)


# all: $(NAME)

# chatclient:
# 	@cp $(SGLD) ./
# 	glib-compile-resources ./src/client.gresource.xml --target=./src/resources.c --generate-source
# 	clang $(SRCS) `pkg-config --cflags --libs gtk+-3.0` -o ./uchat_client -I $(INCS)

# $(FILE:a/%=%)

# $(NAME): $(JSONA) $(OBJS) 
# 	@clang $(CFLG) $(OBJS) -lmx -L$(JSOND) -o $@ $(JSONA)
# 	@printf "\r\33[2K$@ \033[32;1mcreated\033[0m\n"

# $(OBJD)/%.o: $(SRCD)/%.c $(INCS)
# 	@clang $(CFLG) -c $< -o $@ -I$(INCD) -I$(JSONI)
# 	@printf "\r\33[2K$(NAME) \033[33;1mcompile \033[0m$(<:$(SRCD)/%.c=%) "

# $(OBJS): | $(OBJD)

# $(OBJD):
# 	@mkdir -p $@

# $(JSOND): $(JSONA)

# $(JSONA):
# 	@make -sC $(JSOND)

clean:
	@rm -rf ./src/resources.c $(GLD)
	@make -sC $(JSOND) $@
	@rm -rf $(OBJD)
	@printf "$(OBJD)\t   \033[31;1mdeleted\033[0m\n"

uninstall: clean
	@make -sC $(JSOND) $@
	@rm -rf $(NAME)
	@printf "$(NAME) \033[31;1muninstalled\033[0m\n"

reinstall: uninstall all