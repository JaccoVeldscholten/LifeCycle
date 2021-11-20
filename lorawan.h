/*! 
 *  \brief     Header file for LoRaWan SX1276 Driver 
 *  \author    Jacco Veldscholten
 *  \version   1.0
 *  \date      25-5-2021
 *  \pre       Use InitLoRaWan() before any other function
 *  \warning   Improper use can crash your application (TOS Error)
 *  \copyright GNU LifeCycle.
 */
#ifndef LORAWAN_H
#define LORAWAND_H
/***********************************************************************************/
/*                                Dependencies                                     */
/***********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "msg.h"
#include "thread.h"
#include "fmt.h"

#include "periph/rtc.h"

#include "net/loramac.h"
#include "semtech_loramac.h"

#include "sx127x.h"
#include "sx127x_netdev.h"
#include "sx127x_params.h"

#include "xtimer.h"

/***********************************************************************************/
/*                           CONSTANTS (Configuration)                             */
/***********************************************************************************/
#define DEVEUI ""                                   /* DevEUI can be retrieved by KPN IOT */
#define APPEUI ""                                   /* AppEUI can be retrieved by KPN IOT */
#define APPKEY ""                                   /* APPKEY can be retrieved by KPN IOT */

#define MAX_ATTEMPTS_JOIN 5                         /* Amount of attempts to join before activating Message */
#define MAX_ATTEMPTS_SEND 5                         /* Amount of attempts to send before activating Message */

#define LORA_QUEUE 8                                /* LoRa Queue Size for Buffer */
/***********************************************************************************/
/*                          Constant messages  				                       */
/*      	     Its only possible to send the following information    	       */
/***********************************************************************************/
/* The following states are keep in place in the define undernead.                 */
/* Payload must be send as: 1;50 or 0;52.236374;6.837762                           */
/* The webhook will take care of the handling of the parameters                    */
/* 0 = Alarm            (ex: 0;52.236374;6.837762)                                 */
/* 1 = Battery          (ex: 1;50)                                                 */
/* 2 = Register Device  (ex: 2;01)                                                 */                                           
/***********************************************************************************/

#define ALARM_CODE          0
#define BATTERY_CODE        1
#define REGISTER_DEVICE     2
/***********************************************************************************/
/*                              FUNCTION PROTOTYPES                                */
/***********************************************************************************/
void InitLoRaWan(void);
uint8_t SendMsg(void);
void AttemptJoin(void);
void LoraEnqueueRegister(void);
void LoraEnqueueAlarm(char *coords);
void LoraEnqeueCancelAlarm(void);
void LoraEnqueueBattery(uint8_t battery);

extern char message[512];               /* Extern Message for Testing Purpose */

#endif
