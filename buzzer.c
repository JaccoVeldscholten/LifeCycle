/*! 
 *  \brief     Main file for the Buzzer driver
 *  \author    Sil Klaasboer
 *  \version   1.0
 *  \date      25-5-2021
 *  \pre       Call InitBuzzer() before any other functions
 *  \copyright GNU LifeCycle.
 */
#include "buzzer.h"

/*!
Init_PWM() init the PWM device and turn off the pwm
*/
void InitBuzzer(void){
    pwm_init(PWM_BUZZER, PWM_CENTER, DEFAULT_FREQUENCY, DEFAULT_RESOLUTION);
    pwm_set(PWM_BUZZER, DEFAULT_CHANNEL , DEFAULT_RESOLUTION);
    pwm_poweroff(PWM_BUZZER);
}


/*!
Change_Frequency() Changes the frequency channel, dutycycle and resolution after init_PWM()
*/
void ChangeFrequency(int frequency, int duty_cycle, int resolution)
{
   pwm_poweroff(PWM_BUZZER);
   pwm_init(PWM_BUZZER, PWM_CENTER, frequency, resolution);
   pwm_set(PWM_BUZZER, DEFAULT_CHANNEL, duty_cycle);
   pwm_poweron(PWM_BUZZER);
}

/*!
Cycle_between_frequencies() changes the frequency from a begin till an endpoint with a given sleeptime dutycyle stepsize and resolution
*/
void CycleBetweenFrequencies(int begin_freq, int end_freq, int step_size, int sleeptime, int duty_cycle, int resolution)
{
    if(begin_freq < end_freq)
    {
        for(int i = begin_freq; i < end_freq;i = i + step_size)
        {
            ChangeFrequency(i,duty_cycle,resolution);
            xtimer_msleep(sleeptime);
        }
    }
    else
    {
        for(int i = begin_freq; i > end_freq;i = i - step_size)
        {
            ChangeFrequency(i,duty_cycle,resolution);
            xtimer_msleep(sleeptime);
        }
    }
}

/*!
SOS() Makes an SOS signal with threading so other threads can contineu
*/
void SOS(void)
{
    for (int i = 0; i < 9; i++)
    {
        if (i < 3)
        {
            ChangeFrequency(DEFAULT_FREQUENCY, DEFAULT_DUTYCYCLE, DEFAULT_RESOLUTION);
            xtimer_msleep(DEFAULT_SLEEPTIME*2);
            TurnBuzzerOff();
            xtimer_msleep(DEFAULT_SLEEPTIME);
        }
        else if (i < 6)
        {
            TurnBuzzerOn();
            xtimer_msleep(DEFAULT_SLEEPTIME * 4);
            TurnBuzzerOff();
            xtimer_msleep(DEFAULT_SLEEPTIME);
        }
        else
        {
            TurnBuzzerOn();
            xtimer_msleep(DEFAULT_SLEEPTIME*2);
            TurnBuzzerOff();
            xtimer_msleep(DEFAULT_SLEEPTIME);
        }
    }
    xtimer_msleep(DEFAULT_SLEEPTIME*6);
}

/*!
TurnBuzzerOn() Turns on the buzzer
*/
void TurnBuzzerOn(void)
{
    pwm_poweron(PWM_BUZZER);
}

/*!
TurnBuzzerOff() Turns off the buzzer
*/
void TurnBuzzerOff(void)
{
    pwm_poweroff(PWM_BUZZER);
}
