#include "mylib.h"

static void init_prog(prog_name_t* prog, int port){
    
    char my_port[100] = "";
    sprintf(my_port,"tcp://0.0.0.0:%d", port);
    prog->tcp_socket_addr =     strdup(my_port);
    prog->ipc_socket_addr =     IPC_SOCKET;
    prog->inproc_socket_addr =  INPROC_SOCKET;
    prog->ready = true;
    prog->id = 0;
    prog->message = strdup("Hello!");
    prog->name = strdup("process0");
    prog->contex = NULL;

}



static int init_socket(prog_name_t* prog){

    prog->contex = zmq_ctx_new();
    if(prog->contex == NULL){
        printf ("init_socket on contex creation error: %s\n", zmq_strerror(errno));
        return -1;
    }
    return 0;

}

static int create_out_socket(prog_name_t* prog, char* connection_mode){

    prog->socket = zmq_socket(prog->contex, ZMQ_DEALER);

    if (prog->socket == NULL){
        printf ("create out zmq_socket error: %s\n", zmq_strerror(errno));
        return -1;
    }
    if (!strcmp(connection_mode, "ipc")){
        if (zmq_connect(prog->socket, prog->ipc_socket_addr) <0){
            printf ("create ipc connect socket error: %s\n", zmq_strerror(errno));
            return -1;
        }
    }

    if (!strcmp(connection_mode, "inproc")){
        if(zmq_connect(prog->socket, prog->inproc_socket_addr) < 0){
            printf ("create inproc connect socket error: %s\n", zmq_strerror(errno));
            return -1;
        }
    }

    if (!strcmp(connection_mode, "tcp")){
        if (zmq_connect(prog->socket, prog->tcp_socket_addr) <0){
            printf ("create tcp connect socket error: %s\n", zmq_strerror(errno));
            return -1;
        }
    }

    return 0;

}

static int create_in_socket(prog_name_t* prog, char* connection_mode){

    prog->socket = zmq_socket(prog->contex, ZMQ_DEALER);
    
    if (prog->socket == NULL){
        printf ("create in zmq_socket error: %s\n", zmq_strerror(errno));
        return -1;
    }
    if (!strcmp(connection_mode, "ipc")){
        if (zmq_bind(prog->socket, prog->ipc_socket_addr) < 0){
            printf ("create ipc bind socket error: %s\n", zmq_strerror(errno));
            return -1;
        }    
    }

    if (!strcmp(connection_mode, "inproc")){
        if(zmq_bind(prog->socket, prog->inproc_socket_addr) < 0){
            printf ("create inproc bind socket error: %s\n", zmq_strerror(errno));
            return -1;}
    }

    if (!strcmp(connection_mode, "tcp")){
        if (zmq_bind(prog->socket, prog->tcp_socket_addr) < 0){
            printf ("create tcp bind socket error: %s\n", zmq_strerror(errno));
             return -1;
        }
    }
    return 0;

}


static int destroy_out_socket(prog_name_t* prog, char* connection_mode){

   
    if (prog->socket == NULL){
        printf ("destroy zmq out-socket error: %s\n", zmq_strerror(errno));
        return -1;
    }
    if (!strcmp(connection_mode, "ipc")){
        if (zmq_disconnect(prog->socket, prog->ipc_socket_addr) <0){
            printf ("destroy ipc connect socket error: %s\n", zmq_strerror(errno));
            return -1;        
        }
    }
    if (!strcmp(connection_mode, "inproc")){
        if(zmq_disconnect(prog->socket, prog->inproc_socket_addr) < 0){
            printf ("destroy inproc connect socket error: %s\n", zmq_strerror(errno));
            return -1;        
        }
    }
    if (!strcmp(connection_mode, "tcp")){
        if (zmq_disconnect(prog->socket, prog->tcp_socket_addr) <0){
            printf ("destroy tcp connect socket error: %s\n", zmq_strerror(errno));
            return -1;
        }
    }

    return 0;

}

static int destroy_in_socket(prog_name_t* prog, char* connection_mode){


    if (prog->socket == NULL){
        printf ("destroy zmq in_socket error: %s\n", zmq_strerror(errno));
        return -1;
    }
    if (!strcmp(connection_mode, "ipc")){
        if (zmq_unbind(prog->socket, prog->ipc_socket_addr) < 0){
            printf ("destroy ipc bind socket error: %s\n", zmq_strerror(errno));
            return -1;        
        }
    }
    if (!strcmp(connection_mode, "inproc")){
        if(zmq_unbind(prog->socket, prog->inproc_socket_addr) < 0){
            printf ("destroy inproc bind socket error: %s\n", zmq_strerror(errno));
            return -1;        
        }
    }
    if (!strcmp(connection_mode, "tcp")){
        if (zmq_unbind(prog->socket, prog->tcp_socket_addr) < 0){
            printf ("destroy tcp bind socket error: %s\n", zmq_strerror(errno));
            return -1;
        }
    }

    return 0;

}


static int send_message(prog_name_t* prog, message_t* msg, int msg_len, bool multi_part_msg){

    int flag = -1;

    if (multi_part_msg)
        flag = ZMQ_SNDMORE;
    else
        flag = 0;

    if (zmq_send(prog->socket, msg, msg_len, flag) < 0){
        printf ("send error: %s\n", zmq_strerror(errno));
        return -1;
    }
    printf("Sent message: %s, %d, %s\n", msg->prefix,msg->int_val, msg->str_val);

    return 0;
}

static int recv_message(prog_name_t* prog, message_t* buf, int msg_len, int flag){
   
    char time_val[256];
    int _flag = -1;
    
    if (flag == 0)  _flag = 0;
    else if (flag == 1)  _flag = ZMQ_RCVMORE;
    else if (flag == 2)  _flag = ZMQ_NOBLOCK;
    else{
        perror("Invalid flag argument!\n");
        exit(EXIT_FAILURE);
    }
        
    if (zmq_recv(prog->socket, buf, msg_len, flag) < 0){
        printf ("recv error: %s\n", zmq_strerror(errno));
        return -1;
    }
    
    time_t t = time(NULL);
    struct tm *ptm = gmtime(&t);
    strftime(time_val, sizeof time_val, "%F %T", ptm);

    buf->timestamp = strdup(time_val);
    printf("Received message: %s, %d, %s in time: \t\t%s\n", buf->prefix, buf->int_val, buf->str_val, buf->timestamp);
    return 0;
    
}

static int term_socket(prog_name_t* prog){
    
    zmq_close(prog->socket);
    int term = zmq_ctx_destroy (prog->contex);
    if(term < 0)
        printf ("socket contex termination error: %s\n", zmq_strerror(errno));

    
    return 0;
}

static int handler(void* user, const char* section, const char* name,
                   const char* value)
{
    configuration* pconfig = (configuration*)user;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    
    if (MATCH("address", "port")) {
        pconfig->port = atoi(value);
    } else if (MATCH("prefix", "p1")) {
        pconfig->p1 = strdup(value);
    } else if (MATCH("prefix", "p2")) {
        pconfig->p2 = strdup(value);
    } else {
        return 0;  /* unknown section/name, error */
    }
    return 1;
}

static int print_func(void *data, int rows, char* val[], char* key[]){

   //fprintf(stderr, "%s: ", (const char*)data);

   for(int i = 0; i< rows; i++){
      printf("%s = %s\n", key[i], val[i] ? val[i] : "NULL");
   }
   
   printf("\n");
   return 0;
}

char* create_table(){

    char* sql = "CREATE TABLE PASARGAD("  \
                "ID INT PRIMARY KEY    NOT NULL," \
                "PREFIX         CHAR(50)   NOT NULL," \
                "INT_VAL        INT    NOT NULL," \
                "STR_VAL        CHAR(50) NOT NULL);";

    return sql;
}

char* show_table(){

    return "SELECT * FROM PASARGAD";
}

int cmp_func (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}


static void sorting_func(message_t* buffer[]){

    int arr[NUM_OF_MSG / 2];
    memset(arr, 0, sizeof(arr));
    int count = 0;

    for(int i=0; i < NUM_OF_MSG; i++){
        if(buffer[i]->int_val != -1){
            arr[count++] = buffer[i]->int_val;
        }
    }
    qsort(arr, NUM_OF_MSG / 2 , sizeof(int), cmp_func);
    printf("Sorted elements is: \n");
    for (int i = 0; i < 10; i++){
            printf("%d\n", arr[i]);
            }
}