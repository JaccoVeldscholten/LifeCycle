/*! 
 *  \brief     Header file for the GPS driver
 *  \author    Sil Klaasboer
 *  \version   1.0
 *  \date      25-5-2021
 *  \pre       Init usart before calling these functions
 *  \warning   Improper use can crash your application
 *  \copyright GNU LifeCycle.
 */
#ifndef GPS_H
#define GPS_H
/***********************************************************************************/
/*                                Dependencies                                     */
/***********************************************************************************/
#include <stdint.h>
#include "stdio.h"
#include "string.h"
#include "minmea.h"
#include "math.h"
#include "periph/uart.h"
#include "msg.h"
/***********************************************************************************/
/*                                  CONSTANTS                                      */
/***********************************************************************************/
//#define DEBUG_GPS

#define DUMMYLOC "52.00;6.00"               /* Dummy Location for debugging when there is no Fix */
#define PC_UART UART_DEV(0)                 /* Using Device 0 for UART Serial Montior */
#define GPS_UART UART_DEV(1)                /* Using Device 1 for GPS UART */
#define BAUDRATE (9600)
#define BAUDRATE_GPS (38400)
#define UART_BUFSIZE (128U)
#define LAT_LONG_ARRAY_SIZE 20


/***********************************************************************************/
/*                              FUNCTION PROTOTYPES                                */
/***********************************************************************************/

void SetMode(uint8_t c);
int Check_Enter(uint8_t c);
void Print_NMEA_Raw(void);
void Print_NMEA_Format(void);
char* ReturnLastCoords(void);
int ReturnMode(void);

 //WIP
void Send_PMTK(void);
#endif