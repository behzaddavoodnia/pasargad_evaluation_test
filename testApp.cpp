#include <stdio.h>
#include <stdlib.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mylib.h"
#include "myprog.c"

using ::testing::Return;

TEST (PasagatTestSuit, testAllAPIs){
    prog_name_t* p1 = (prog_name_t*) calloc (100, sizeof(prog_name_t));
    message_t* msg = (message_t*) calloc(100, sizeof (message_t));
    init_prog(p1, 9000);
    ASSERT_NO_FATAL_FAILURE(p1);
    ASSERT_EQ(init_socket(p1),0);
    ASSERT_EQ(NUM_OF_MSG, 20);
    ASSERT_EQ(create_out_socket(p1, "tcp"), 0); 
    ASSERT_EQ(create_in_socket(p1, "tcp"), 0); 
    //ASSERT_NO_FATAL_FAILURE(send_message(p1, msg, sizeof(message_t), false));
    //ASSERT_NO_FATAL_FAILURE(recv_message(p1, msg, sizeof(message_t), 0));
    ASSERT_EQ(destroy_out_socket(p1, "tcp"), 0);
    ASSERT_TRUE(create_table());
    ASSERT_TRUE(show_table());
    ASSERT_NO_FATAL_FAILURE(safe_free(p1));
    ASSERT_NO_FATAL_FAILURE(safe_free(msg));
     
}

int main(int argc, char* argv[]){

    testing::InitGoogleTest(&argc, argv);

return RUN_ALL_TESTS();
    
}