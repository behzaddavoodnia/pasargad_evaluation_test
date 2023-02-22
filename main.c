#include "mylib.h"
#include "myprog.c"

int main(int argc, char** argv){

//system("mkdir "SOCKET_DIR);
//system("sudo rm test.db");
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
            msg[i]->timestamp = strdup("");
        }
        else {
            char my_str[128] = "";
            msg[i]->prefix = strdup("p2");
            msg[i]->int_val = -1;
            sprintf(my_str,"Pasargad%d",i);
            msg[i]->str_val = strdup(my_str);
            msg[i]->timestamp = strdup("");
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
        sleep(rand()%3);
   }
    //term_socket(p2);

//=======================================================


   /* Open database */
   rc = sqlite3_open("test.db", &db);
   if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return(0);
   } else {
      fprintf(stdout, "Opened database successfully\n");
   }

   /* Create SQL table */
   new_sql = create_table();
   
    if (create_table() != NULL){
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
    
    sorting_func(buffer);

    safe_free(p1);
    safe_free(p2);


    return 0;
}
