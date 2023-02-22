#include "zmq_basic.h"

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
    
    printf("Received message: %s, %d, %s\n", buf->prefix, buf->int_val, buf->str_val);
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


 

int main(int argc, char** argv){

//system("mkdir "SOCKET_DIR);
   sqlite3* db;
   char* err_msg = "";
   int rc;
   char sql[256] = "";
   char* new_sql = "";
   const char* data = "print function called\n";


/* Create message and initilize it*/
    message_t* msg[NUM_OF_MSG];
    for(int i=0; i< NUM_OF_MSG; i++){
        msg[i] = (message_t*) malloc(NUM_OF_MSG * sizeof (message_t));
    }
    srand(time(NULL));

    for (int i=0; i < NUM_OF_MSG; i++){
        if ((i % 2) == 0){
            msg[i]->prefix = strdup("p1");
            msg[i]->int_val = rand() % INT_VAL_RANGE;
            msg[i]->str_val = strdup("NULL");
        }
        else {
            char my_str[128] = "";
            msg[i]->prefix = strdup("p2");
            msg[i]->int_val = -1;
            sprintf(my_str,"Pasargad%d",i);
            msg[i]->str_val = strdup(my_str);
        }
    printf("msg %d is: %s, %d, %s\n", i, msg[i]->prefix, msg[i]->int_val, msg[i]->str_val);

    }

/* Create or read the ini config file */
    configuration config;
    if (ini_parse("fconfig.ini", handler, &config) < 0) {
        printf("Can't load 'fconfig.ini'\n");
        return 1;
    }
    printf("\nConfig loaded from 'fconfig.ini':\nport = %d, p1 = %s, p2 = %s\n\n",
        config.port, config.p1, config.p2);

/* Create and init process 1 (program 1) as a sender and get port number from ini file*/
    prog_name_t* p1 = (prog_name_t*) calloc (100, sizeof(prog_name_t));
    init_prog(p1,config.port);
    init_socket(p1);
    //create_out_socket(p1, "ipc");
    create_out_socket(p1, "tcp");
    
    for(int i=0; i < NUM_OF_MSG -1 ; i++){
        //sleep(1);
        send_message(p1, msg[i], sizeof(message_t), true);
    }
    //sleep(1);
    send_message(p1, msg[19], sizeof(message_t), false); // Determine the last message in socket

/* Create and init process 2 (program 2) as a reciver and get port number from ini file*/
    prog_name_t* p2 = (prog_name_t*) calloc (100, sizeof(prog_name_t));
    message_t* buffer[NUM_OF_MSG];
    
    for(int i=0; i< NUM_OF_MSG; i++){
        buffer[i] = (message_t*) malloc(NUM_OF_MSG * sizeof (message_t));
    }
    
    init_prog(p2,config.port);
    init_socket(p2);
    //create_in_socket(p2, "ipc");
    create_in_socket(p2, "tcp");
    
   printf("\n"); 
   for(int i=0; i < NUM_OF_MSG ; i++){
        //sleep(1);
        recv_message(p2, buffer[i], sizeof (buffer), 0);
   }
    //term_socket(p2);

//=======================================================


   /* Open database */
   rc = sqlite3_open("test.db", &db);
   if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return(0);
   } else {
      fprintf(stdout, "Opened database successfully\n\n");
   }

   /* Create SQL table */
   new_sql = create_table();
   
    if (create_table(new_sql) != NULL){
        sqlite3_exec(db, new_sql, print_func, (void*) data, &err_msg);
        printf("Table created successfully!\n");
    }
    else 
        fprintf(stderr, "Can't create table");

    /* Insert buffer into the table */
    for (int i =0; i < NUM_OF_MSG; i++){
        sprintf(sql,"INSERT INTO PASARGAD (ID,PREFIX,INT_VAL,STR_VAL)" "VALUES \
        (%d, '%s', %d, '%s');", i, buffer[i]->prefix, buffer[i]->int_val, buffer[i]->str_val);
        int rc = sqlite3_exec(db, sql, print_func, (void*) data, &err_msg);
        if (!rc)
            fprintf(stderr,"Error on inserting into the table!\n");
    }

    /* Show the table */
    new_sql = show_table();
    rc = sqlite3_exec(db, new_sql, print_func, (void*) data, &err_msg);
   
    if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", err_msg);
      sqlite3_free(err_msg);
    } 
    else {
      fprintf(stdout, "Program finished successfully\n");
    }

    sqlite3_close(db);
    safe_free(p1);
    safe_free(p2);


    return 0;
}
