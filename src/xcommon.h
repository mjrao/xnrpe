#ifndef __XCOMMON_H
#define __XCOMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <assert.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>
#include <sys/time.h>
#include <stdarg.h>

#define true 1
#define false 0

#define OK 0
#define ERROR -1
#define MAX_LISTEN_SOCKS 16
#define NI_MAXHOST 1025
#define DEFAULT_SERVER_PORT  5566
#define DEFAULT_SOCKET_TIMEOUT 60
#define DEFAULT_CONNECTION_TIMEOUT 300
#define EPOLL_LEN 20480
#define MAX_INPUT_BUFFER 2048
#define MAX_FILENAME_LENGTH 256
#define DEFAULT_SOCK_SEND_RECV_TIMEOUT 10 //seconds
#define DEFAULT_HEARTBEAT_TIME 5
#define MAX_COMMAND_NUM 1024
#define MESSAGE_HEAD_LEN 7
#define MAX_SYSTEM_RETRUN_BUFFER  819200
#define LOG_MAX_LEN  1024
#define CONFIG_DEFAULT_PID_FILE "/var/run/xnrpe.pid"
#define LOG_DEFAULT_FILE "/var/log/xnrpe.log"

//#define _XNRPE_DEBUG 1
//#define _TEST   1

#define LL_DEBUG 0
#define LL_VERBOSE 1
#define LL_NOTICE 2
#define LL_WARNING 3
#define LL_RAW (1<<10) 

extern char server_address[NI_MAXHOST];
extern int server_port;
extern int sock_send_recv_timeout;
extern int report_time;
extern int heartbeat_time;
extern char local_host[NI_MAXHOST];
extern char pid_file_path[MAX_INPUT_BUFFER];

extern char *log_file;

extern int command_array_size;
extern int report_flag;
extern int daemonize;
extern int gdebug;

typedef struct args{
    int time; //seconds
    char cmd[MAX_INPUT_BUFFER];
}ARGS;

typedef struct command_struct{
    char *command_name;
    char *command_line;
    struct command_struct *next;
}command;

//task command
typedef  struct command_task_struct{
    char neType[512];
    char neId[512];
}command_task;


extern command_task commands_array[MAX_COMMAND_NUM];

int process_arguments(int argc, char **argv);

int setnonblocking(int sock);
int resolve_host_name(char* hostname, struct in_addr* addr);

int start_connect(char *host, int port);

/*****send & read buff format*******/
/*
* 2 byte  begin header
* 4 byte  json data length
* 1 byte  op type:  0 heart beat, 1 report message
*|---|-------|-|----------|
*|0|0|0|0|0|0|0|json data |
*|---|-------|-|----------|
*/

int report_tcp_information(char *info , int len, int recv_flag);
int send_tcp_all(int s,char *buf, int len);
int read_response(int sock, char *buf);
int handle_response_message(char *buf, int len);
int pack_msg(char *inbuf, unsigned int len, char *outbuf,char type);


//define in xnrpe.c
void fun_int(int arg);
void fun_term(int arg);
void handle_pipe(int sig);
void *task(void *args);
void usage(int result);
int process_arguments(int argc, char **argv);
int add_command(char *command_name, char *command_line);
command *find_command(char *command_name);
int read_config_file(char *filename);
void free_memory(void);

int my_system(char *command, char *outbuf);


/********hand heartbeat respon json msg********/
/*
json example: [{"command":"check_disk","id":"001"}]
*/
int handle_heartbeat_respon_msg(char *str);


/////xnrpe.c functions//////
void create_pid_file();
void create_daemonize();

void logit_raw(int level, const char *msg);
void logit(int level, const char *fmt, ...);
#endif // __XCOMMON_H
