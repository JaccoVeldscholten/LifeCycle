/*! 
 *  \brief     Functions file for LoRaWan SX1276 Driver 
 *  \author    Jacco Veldscholten
 *  \version   1.0
 *  \date      25-5-2021
 *  \pre       Use InitLoRaWan() before any other function
 *  \warning   Improper use can crash the application (TOS Error)
 *  \copyright GNU LifeCycle.
 */
#include "lorawan.h"

static uint8_t deveui_conv[LORAMAC_DEVEUI_LEN * 2];
static uint8_t appeui_conv[LORAMAC_APPEUI_LEN * 2];
static uint8_t appkey_conv[LORAMAC_APPKEY_LEN * 2];

static semtech_loramac_t loramac;
static sx127x_t sx127x;

char message[512];
uint8_t joinFailed = 0;


/*!
InitLoRaWan() Set up the SX1276 Driver and attempt to Join the KPN Network 	
*/
void InitLoRaWan(void){
    printf("LoRaWAN Init SX1276 Driver \n");
    printf("===================================== \n");

    fmt_hex_bytes(deveui_conv, DEVEUI);      /*  Convert String Key to Bytes */
    fmt_hex_bytes(appeui_conv, APPEUI);      /*  Convert String Key to Bytes */
    fmt_hex_bytes(appkey_conv, APPKEY);      /*  Convert String Key to Bytes */

    /* Initialize the radio driver */
    sx127x_setup(&sx127x, &sx127x_params[0], 0);            /* Call SX1276 Driver */
    loramac.netdev = (netdev_t *)&sx127x;
    loramac.netdev->driver = &sx127x_driver;

    /* Initialize the loramac stack */
    semtech_loramac_init(&loramac);
    semtech_loramac_set_deveui(&loramac, deveui_conv);          /* Flash the unused slot if magic word is set */
    semtech_loramac_set_appeui(&loramac, appeui_conv);          /* Flash the unused slot if magic word is set */
    semtech_loramac_set_appkey(&loramac, appkey_conv);          /* Flash the unused slot if magic word is set */
    
    /* Use a fast datarate, e.g. BW125/SF7 in EU868  (This is important for KPN/TTN)*/
    semtech_loramac_set_dr(&loramac, LORAMAC_DR_5);     

    AttemptJoin();
    LoraEnqueueRegister();                   /* Prepare Register Device Message */
}

/*!
SendMsg() Sends the message by LoraMac and verifys if it succeed  
*/

uint8_t SendMsg(void)
{
    /* Attempt to send Message to KPN 
        Sends Back 1 If succseed and 0 if failed 
        The Fault_Code is for debug purpose 
    */
    uint8_t fault_code = semtech_loramac_send(&loramac,(uint8_t *)message, strlen(message));
    
    if (fault_code != SEMTECH_LORAMAC_TX_DONE)  {
        printf("[LORA] Cannot send message '%s', fault code: %d\n", message, fault_code);
        return 0;
    }
    return 1;
}

/*! AttemptJoin()  				                      
        This function will attempt to join the OTAA Network.                    
        If it fails it will attempt to join in 2000ms Again                    
        In case that failes it will exit the application for now. Assuming wrong keys used
*/

void AttemptJoin(void){
    printf("# Attempt to Join # \n");
    while (semtech_loramac_join(&loramac, LORAMAC_JOIN_OTAA) != SEMTECH_LORAMAC_JOIN_SUCCEEDED) {
        if(joinFailed++ >= MAX_ATTEMPTS_JOIN){
            printf("[LORA] Keys not correct. Or no KPN Coverage. Join failed");
            //exit(-1);         /* For Debug Purpose */                
        }
		xtimer_msleep(2000);    /* Respect the duty cycle of LoRaWan Wait 1 second before sending */
    }
    printf("Join succesfull");
}

/*!
LoraEnqueueRegister()  	Register for the first time on the network and database by hook
*/

void LoraEnqueueRegister(void){
    sprintf(message, "%d;00", REGISTER_DEVICE); 
    xtimer_msleep(1000);                 /* Respect the duty cycle of LoRaWan Wait 1 second before sending */
    SendMsg();		                     /* Instantly send out the register so it will not be threaded */
}

/*!
LoraEnqueueAlarm()  	Register for the first time on the network and database by hook
*/

void LoraEnqueueAlarm(char *coords){
    sprintf(message, "%d;%s", ALARM_CODE, coords); 
}

/*!
LoRaEnqeueCancelAlarm()  	Send out the Alarm state when someone falled with the coords format: 0;0;0
*/

void LoraEnqeueCancelAlarm(void){
    sprintf(message, "%d;0;0", ALARM_CODE); 
}

/*!
LoraEnqueueBattery()  Send message to Database that it is active and log the battery
*/
void LoraEnqueueBattery(uint8_t battery){
    sprintf(message, "%d;%d", BATTERY_CODE, battery); 
}
