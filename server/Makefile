NAME	=	server

CFLG	=	-std=c11 -lsqlite3

SRCD	=	src
INCD	=	inc
OBJD	=	obj

INC		=	server.h
INCS	=	$(addprefix $(INCD)/, $(INC))

SRC		=

SRCS	=	$(addprefix $(SRCD)/, $(SRC))
OBJS	=	$(addprefix $(OBJD)/, $(SRC:%.c=%.o))

all: $(NAME)

$(NAME):
	clang $(CFLG) src/*.c -I inc/ -I -o