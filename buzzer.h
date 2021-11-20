/*! 
 *  \brief     Header file for the Buzzer driver
 *  \author    Sil Klaasboer
 *  \version   1.0
 *  \date      25-5-2021
 *  \pre       Call InitBuzzer() before any other functions
 *  \warning   Improper use can crash your application
 *  \copyright GNU LifeCycle.
 */
#ifndef BUZZER_H
#define BUZZER_H
/***********************************************************************************/
/*                                Dependencies                                     */
/***********************************************************************************/
#include "periph/pwm.h"
#include <stdbool.h>
#include "xtimer.h"

/***********************************************************************************/
/*                                  CONSTANTS                                      */
/***********************************************************************************/
#define PWM_BUZZER PWM_DEV(0)
#define DEFAULT_FREQUENCY 500
#define DEFAULT_RESOLUTION 100
#define DEFAULT_DUTYCYCLE 50
#define DEFAULT_CHANNEL 1
#define DEFAULT_SLEEPTIME 100


/***********************************************************************************/
/*                              FUNCTION PROTOTYPES                                */
/***********************************************************************************/

void ChangeFrequency(int frequency, int duty_cycle, int resolution);
void CycleBetweenFrequencies(int begin_freq, int end_freq, int step_size, int sleeptime, int duty_cycle, int resolution);
void SOS(void);
void TurnBuzzerOn(void);
void TurnBuzzerOff(void);
void InitBuzzer(void);
#endif
