NAME = libjsonc.a

SRC = arraylist.c \
	debug.c \
	json_c_version.c \
	json_object.c \
	json_object_iterator.c \
	json_pointer.c \
	json_tokener.c \
	json_util.c \
	json_visit.c \
	libjson.c \
	linkhash.c \
	printbuf.c \
	random_seed.c \
	strerror_override.c \

SRCS = $(addprefix src/, $(SRC))

OBJ_DIR = obj

OBJS = $(SRCS:src/%.c=obj/%.o)

CFLAGS = -std=c11 -Wall 

all: install

install: $(NAME)

$(OBJ_DIR):
	@mkdir -p $@

$(NAME): $(OBJ_DIR) $(OBJS)
	@ar rcs $(NAME) $(OBJS)

$(OBJ_DIR)/%.o: src/%.c
	@clang $(CFLAGS) -c $< -o $@

uninstall:
	@rm -rf $(NAME)

clean:
	@rm -rf ./obj

reinstall: uninstall install
