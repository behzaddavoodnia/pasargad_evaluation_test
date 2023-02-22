#ifndef ZMQ_BASIC_H
#define ZMQ_BASIC_H


/*ZMQ_REQ does not throw away any messages. If there are no available services to send
the message or if the all services are busy, all send operations i.e. zmq_send(), are
blocked until a service becomes available to send the message*/


// System headers
#include <stdint.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <msgpack.h>
#include <zmq.h>
#include <czmq.h>
#include <pthread.h>
#include <unistd.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sqlite3.h>
#include "ini.c"
//#include "ini.h"


// Deallocating a memory pointed by ptr causes dangling and non-wild pointer
#ifndef safe_free
#define safe_free(ptr) ({free(ptr); ptr=NULL;})
#endif
 

// +----------------------- Socket Paths ------------------------+ //

#define SOCKET_DIR      			"/tmp/feeds/"
#define IPC_SOCKET			        "ipc:///tmp/feeds/0"
#define IPC_SOCKET_INPUT		    "ipc://%s"
#define TCP_SOCKET		            "tcp://0.0.0.0:5555"
#define TCP_SOCKET_INPUT	        "tcp://%s:%s"
#define TCP_SOCKET_ALL	            "tcp://*:5555"
#define INPROC_SOCKET               "inproc://my-inproc-endpoint"
#define INPROC_SOCKET_INPUT			"inproc://%s"

#define MAX_SIZE_OF_NAME            256

#define NUM_OF_MSG                  20
#define INT_VAL_RANGE               100


typedef struct _prog_name_t
{
    char* name;
    char* message;
    char* ipc_socket_addr;
    char* inproc_socket_addr;
    char* tcp_socket_addr;
    unsigned id;
    void* contex;
    void* socket;
    bool ready;
    
}prog_name_t;

typedef struct _message_t
{
    int int_val;
    char* str_val;
    char* prefix;
}message_t;


typedef struct _configuration {
    int port;
    char* p1;
    char* p2;
} configuration;



static void init_prog(prog_name_t* prog, int port);

static int init_socket(prog_name_t* prog);

static int create_out_socket(prog_name_t* prog, char* connection_mode);

static int create_in_socket(prog_name_t* prog, char* connection_mode);

static int destroy_out_socket(prog_name_t* prog, char* connection_mode);

static int destroy_in_socket(prog_name_t* prog, char* connection_mode);

static int send_message(prog_name_t* prog, message_t* msg, int msg_len, bool multi_part_msg);

static int recv_message(prog_name_t* prog, message_t* msg, int msg_len, int flag);

static int term_socket(prog_name_t* prog);

static int handler(void* user, const char* section, const char* name, const char* value);

static int print_func(void *data, int rows, char* val[], char* key[]);


#endif // !ZMQ_BASIC_H