/*! 
 *  \brief     Header file for the oled driver
 *  \author    Sil Klaasboer
 *  \version   1.0
 *  \date      25-5-2021
 *  \pre       Use InitI2cDisplay() before any other function
 *  \warning   Improper use can crash your application
 *  \copyright GNU LifeCycle.
 */
#ifndef OLED_H
#define OLED_H
/***********************************************************************************/
/*                                Dependencies                                     */
/***********************************************************************************/
#include <stdint.h>
#include "stdio.h"
#include "string.h"
#include "u8g2.h"
#include "xtimer.h"
#include "u8x8_riotos.h"
#include "u8x8.h"
/***********************************************************************************/
/*                                  CONSTANTS                                      */
/***********************************************************************************/
//#define DEBUG_OLED


#define I2C_DEVICE  0
#define I2C_ADDR  0x3c
#define I2C_PIN_CS  GPIO_UNDEF
#define I2C_PIN_DC  GPIO_UNDEF
#define I2C_PIN_RESET  GPIO_UNDEF
#define I2C_DISPLAY u8g2_Setup_ssd1306_i2c_128x32_univision_f

/***********************************************************************************/
/*                              FUNCTION PROTOTYPES                                */
/***********************************************************************************/

void InitI2cDisplay(void);  
void ClearBuffer(void);
void FirstPage(void);
void SetDrawColor(uint8_t color);
void SetFont(const uint8_t *font);
void DrawString(uint8_t x, uint8_t y, const char *s);
void Draw1Line(uint8_t x, uint8_t y, const char *s);
void Draw2Lines(uint8_t x1, uint8_t y1, const char *s1, uint8_t x2, uint8_t y2, const char *s2);
void DrawBattery(int percentage);
void CyclePercentage(void);
void SetPowerSave(uint8_t mode);
void ClearDisplay(void);


#endif