#include "tests-lorawan.h"


void test_LoRa_Should_Send_Message(void){
    /* This Test Will try to send a Message By Lora */
    /* The LoRa device is a Mock */
    TEST_ASSERT_EQUAL(0, SendMsg());
}


void test_LoRa_Should_Register(void){
    /* This Test Updates the LoRa Message buffer */
    /* Expected Result: 2;00 */ 
    LoraEnqueueRegister();
    TEST_ASSERT_EQUAL_STRING(message, "2;00");
}

void test_LoRa_Should_Alarm(void){
    /* This Test Updates the LoRa Message buffer */
    /* Expected Result: 0;52.236374;6.837762 */ 
    LoraEnqueueAlarm("52.236374;6.837762");
    TEST_ASSERT_EQUAL_STRING(message, "0;52.236374;6.837762");
}

void test_LoRa_Should_Cancel_Alarm(void){
    /* This Test Updates the LoRa Message buffer */
    /* Expected Result: 0;0;0 */ 
    LoraEnqeueCancelAlarm();
    TEST_ASSERT_EQUAL_STRING(message, "0;0;0");
}

void test_LoRa_Should_Update_Battery(void){
    /* This Test Updates the LoRa Message buffer */
    /* Expected Result: 1;23 */ 
    LoraEnqueueBattery(23);
    TEST_ASSERT_EQUAL_STRING(message, "1;23");
}
