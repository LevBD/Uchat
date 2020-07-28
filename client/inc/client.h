#ifndef CLIENT_H
#define CLIENT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/errno.h>
#include <pthread.h>
#include <netdb.h>
#include <unistd.h>
#include <stdbool.h>
#include "../../libjson/json.h"
#include <gtk/gtk.h>
#include <time.h>
#include <gio/gio.h>
#include <err.h>
#include "../../libmx/inc/libmx.h"
#include "../../libressl-3.2.0/include/tls.h"
#include "../../libressl-3.2.0/include/openssl/evp.h"
#include "../../libressl-3.2.0/include/openssl/aes.h"
#include "../../libressl-3.2.0/include/openssl/sha.h"
#include "../../libressl-3.2.0/include/openssl/conf.h"


#define BUF_SIZE 2048
#define REQUEST_HISTORY 10

/* ======= Sing In defines ======== */
#define LG_ERROR_DATA "Wrong login or password"
#define LG_ERROR_CONNECTION "Bad Internet connection"
#define LG_ERROR_ONLINE "User already online"

// ======= Sing Up defines ========
#define SU_ERROR_DATA_LOGIN "Login already exist"
#define SU_ERROR_DATA_NICKN "Nickname already exist"
#define SU_ERROR_CONNECTION "Bad Internet connection"
#define SU_ERROR_CONFIRM_PASS "Wrong password confirm"

// ============ Reconnecting defines ============== //
#define CNCT_AM 5       // AMOUNT TRY
#define CNCT_CLDN 5     // COOL DOWN


GtkWidget *chatWindow;
GtkWidget *sendEntry, *sendButton;
GtkWidget *statusLabel;
GtkWidget *Viewport;
GtkAdjustment *vAdjust;
GtkScrolledWindow *scrolledWindow;
GtkListBox *messageList;
pthread_t watcher;
pthread_t server;
gulong hendler_id_entry;
gulong hendler_id_button;

void mx_init_chat_window();
// extern GtkWidget *chatWindow;

void mx_init_login_window();
extern GtkWidget *loginWindow;

void mx_init_sign_up_window();
extern GtkWidget *signUpWindow;

void mx_init_start_window();
extern GtkWidget *StartWindow;

typedef struct s_row t_row;

/* =============== MESSAGE STRUCTURE =============== */

typedef struct s_user_message
{
    unsigned int owner_id;
    char *data;
    time_t tv_id;
    const char *nickname;
    GtkWidget *row;
} t_user_message;

/* ============= MESSAGE STRUCTURE END ============= */

/* =============== USER STRUCTURE =============== */

typedef struct s_start {
    int port;
    char *ip;
} t_start;

typedef struct s_row
{
    GtkWidget *popup_menu;
    GtkWidget *row;
    GtkWidget *in_row;
    GtkWidget *nickname;
    GtkWidget *time;
    GtkWidget *message_label;
    GtkWidget *message;
    GtkWidget *void_box;
    GtkWidget *container;
    GtkWidget *tool_box;
    GtkWidget *edit;
    GtkWidget *delete;
    GtkWidget *ebox;
}t_row;

typedef struct s_user_info
{
    char *login;
    bool is_online;
    char *password;
    char *confpass;
    const char *nickname;
    unsigned int id;
    char *last_server_back;
} t_user_info;

/* ============= USER STRUCTURE END ============= */

t_user_info owner;
t_list *history_message_list;
struct tls_config *tls_cfg;
struct tls *tls_ctx;
t_start *start_data;
bool is_editing;
volatile bool lock;
// GtkWidget *widget_list;


typedef struct s_client_info {
    char **argv;
    int socket;
} t_client_info;

typedef struct s_edit_data {
    t_user_message *message;
    t_user_message *new_message;
    int index;
} t_edit_data;

t_client_info *mx_get_client_info(void);

/* ================== SERVER HANDLER ================== */

void *mx_proc_server_back(char *buffer, t_user_info *user);
t_user_message *mx_proc_message_back(json_object *jobj);
void mx_do_message_request(t_user_message *message, const char *request);
void *mx_read_server_thread(void *par);
void mx_switch_message_back(t_user_info *user, t_user_message *new_message);

/* =================== SIGN UP ====================== */

char *mx_proc_sign_up_back(json_object *jobj);
char *mx_message_do_sing_up(t_user_info *reg_par);
void *mx_sing_up_thread(void *param);
gboolean mx_check_reg(void *param);
void mx_do_reg(GtkWidget *widget, gpointer data);

/* =================== SIGN IN ====================== */

char *mx_message_do_login(t_user_info *log_par);
void *mx_login_thread(void *param);
void mx_do_login(GtkWidget *widget, gpointer data);
gboolean mx_check_login(void *param);
char *mx_proc_log_in_back(json_object *jobj, t_user_info *user);

/* ================== GTK EXTRA ===================== */

gboolean mx_autoscroll();
void mx_change_title(char *name);
int mx_show_popup(GtkWidget *widget, GdkEvent *event);
void mx_css_set(GtkCssProvider *cssProvider, GtkWidget *widget);
void mx_change_status(char *temp);
gboolean mx_grab_focus(GtkWidget *temp);

/* ==================== EXTRA ======================= */

bool mx_is_message_by_data(t_list_node *node, int owner_id, int message_id, t_user_message **message);
char *mx_short_time(time_t *t);
void mx_zero_string(char *arr);
void *mx_find_row_from_list__row(void *widget);
void mx_fill_user(t_user_info *to, t_user_info *from);

/* ==================== FREE ======================== */

void mx_messdel(t_user_message **message);
void mx_clear_node(t_list_node *node);

/* ================ GTK HISTORY ===================== */

void mx_listbox_node_cleaning(t_list_node *node);
gboolean mx_listbox_cleaning();
GtkWidget *mx_create_out_mess(const char *message_text, const char *login_text, time_t time_v);
GtkWidget *mx_create_in_mess(const char *message_text, const char *login_text, time_t time_v);

/* ================== HISTORY ======================= */

void mx_message_request_history(void);
void mx_do_history_request();
t_list *mx_create_hst_message_list(int list_size);
void mx_do_history_ready();
int mx_history_size(json_object *jobj);

void mx_clear_history(void);
int mx_get_index_history_message(t_list *list, int owner_id, int message_id, t_user_message **message);

/* =================== ADD ========================= */

gboolean mx_add_message_widget(t_user_message *new_message);
void mx_do_send();
void *mx_watcher_thread(void *param);

/* ================== DELETE ======================= */

void mx_delete_mess(GtkWidget *widget, gpointer data);
void mx_delete_message_row(t_user_message *message);
gboolean mx_delete_message(t_user_message *new_message);

/* ================== EDIT ======================= */

void mx_edit_message_complite(GtkWidget *widget, gpointer data);
t_user_message *mx_create_edit_message(t_user_message *message, char *msg_body);
void mx_edit_mess(GtkWidget *widget, gpointer data);
gboolean mx_edit_message(t_edit_data *edit);

/* =============== TLS =============== */
void mx_report_tls(struct tls * tls_ctx, char * host);

/* =========== RECONNECTION ========== */
int mx_init_connection(int sock, t_start *start_data);
int mx_do_reconnection(int rc);

// void print_history();

#endif
