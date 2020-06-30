NAME	=	client

CFLG	=	`pkg-config --cflags --libs gtk+-3.0` -std=c11

all: $(NAME)

$(NAME):
	clang $(CFLG) src/*.c -I inc/ -I -o
