COMMON_FILES=../proto/proto.c
CLIENT_FILES=resources.c start.c sign_up.c login.c chat.c  main.c

all: chatclient
chatclient:
	glib-compile-resources client.gresource.xml --target=resources.c --generate-source
	gcc  $(CLIENT_FILES) `pkg-config --cflags --libs gtk+-3.0` -o ./chat_client
clean:
	rm chat_client resources.c
