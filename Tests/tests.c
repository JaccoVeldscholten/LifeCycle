#include "tests.h"


void setUp(void){

}

void tearDown(void){
    
}



int TestSuite(void)
{
    printf("[UNIT TEST] Starting UNIT Testing \n");
    UNITY_BEGIN();
    printf("[UNIT TEST] LoRaWan Tests \n");
    RUN_TEST(test_LoRa_Should_Send_Message);
    RUN_TEST(test_LoRa_Should_Register);
    RUN_TEST(test_LoRa_Should_Alarm);
    RUN_TEST(test_LoRa_Should_Cancel_Alarm);
    RUN_TEST(test_LoRa_Should_Update_Battery);

    printf("[UNIT TEST] GPS Tests \n");
    RUN_TEST(test_GPS_check_enter);
    RUN_TEST(test_GPS_mode_should_change);
    RUN_TEST(test_GPS_return_last_coords);
    

    printf("[UNIT TEST] MMA Tests \n");
    RUN_TEST(test_MMA_Should_Calculate_G);
    
    return UNITY_END();
}