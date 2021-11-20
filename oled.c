/*! 
 *  \brief     Main file for the oled driver
 *  \author    Sil Klaasboer
 *  \version   1.0
 *  \date      25-5-2021
 *  \pre       Use InitI2cDisplay() before any other function
 *  \warning   Improper use can crash your application
 *  \copyright GNU LifeCycle.
 */
#include "oled.h"
u8g2_t u8g2;
u8x8_riotos_t user_data =
	{
		.device_index = I2C_DEVICE,
		.pin_cs = I2C_PIN_CS,
		.pin_dc = I2C_PIN_DC,
		.pin_reset = I2C_PIN_RESET,
};

/*!
init_i2c_display() init the display and turn off powersave
*/
void InitI2cDisplay(void){
    #ifdef DEBUG_OLED
    printf("[OLED] Initializing I2C OLED\n");
    #endif
    I2C_DISPLAY(&u8g2, U8G2_R0, u8x8_byte_hw_i2c_riotos, u8x8_gpio_and_delay_riotos);
	u8g2_SetUserPtr(&u8g2, &user_data);
	u8g2_SetI2CAddress(&u8g2, 0x3c);
	u8g2_InitDisplay(&u8g2);
	//u8g2_SetPowerSave(&u8g2, 0);
    
}

/*!
SetPowerSave() sets the current powersave mode
*/
void SetPowerSave(uint8_t mode)
{
    #ifdef DEBUG_OLED
    printf("[OLED] Setting powersave mode to : %d\n",mode);
    #endif
    u8g2_SetPowerSave(&u8g2,mode);
}

/*!
Cycle_percentage() displays 0 to 100% on the display
*/
void CyclePercentage(void)
{
    #ifdef DEBUG_OLED
    printf("[OLED] Cycle power precentage\n");
    #endif
    for(int i = 0; i < 100; i++)
    {
        DrawBattery(i);
        xtimer_msleep(1000);
    }
}

/*!
ClearBuffer() clears the display buffer on the oled screen
*/
void ClearBuffer(void)
{
    #ifdef DEBUG_OLED
    printf("[OLED] Clearing display buffer\n");
    #endif
    u8g2_ClearBuffer(&u8g2);
}

/*!
FirstPage() Resets the display pixel pointer to the first row of the display
*/
void FirstPage(void)
{
    u8g2_FirstPage(&u8g2);
}

/*!
SetDrawColor() Sets the current Draw Color (1 is whithe 0 is black)
*/
void SetDrawColor(uint8_t color)
{
    #ifdef DEBUG_OLED
    printf("[OLED] Drawing color set to%d\n",color);
    #endif
    u8g2_SetDrawColor(&u8g2,color);
}

/*!
SetFont() Sets the current Font for fonts check: https://github.com/olikraus/u8g2/wiki/fntlistall
*/
void SetFont(const uint8_t *font)
{
    #ifdef DEBUG_OLED
    printf("[OLED] Font has been set\n");
    #endif
    u8g2_SetFont(&u8g2, font);
}

/*!
DrawString() prints the current char * with the given x and y coords to the display
*/
void DrawString(uint8_t x, uint8_t y, const char *s)
{
    u8g2_DrawStr(&u8g2,x,y,s);
}

/*!
Draw_1_Line() draws 1 string on the display with the given x and y coords
*/
void Draw1Line(uint8_t x, uint8_t y, const char *s)
{
        ClearBuffer();
        SetDrawColor(1);
	    u8g2_DrawStr(&u8g2,x,y,s);
        u8g2_SendBuffer(&u8g2);
        

}

/*!
Draw_2_Lines() draws 2 strings on the display with the given x and y coords for each string
*/
void Draw2Lines(uint8_t x1, uint8_t y1, const char *s1, uint8_t x2, uint8_t y2, const char *s2)
{
        u8g2_FirstPage(&u8g2);  
    	do
		{
			u8g2_DrawStr(&u8g2,x1,y1,s1);
            u8g2_DrawStr(&u8g2,x2,y2,s2);
            
		} while( u8g2_NextPage(&u8g2) );
}

/*!
ClearDisplay() clears the internal buffer of the display and the display it self
*/
void ClearDisplay(void)
{
    u8g2_ClearDisplay(&u8g2);
}

/*!
Draw_Battery() prints a battery icon with a percentage and amout of charge within the battery on the screen
*/
void DrawBattery(int percentage)     
{       
        #ifdef DEBUG_OLED
            printf("[OLED] Drawing battery percentage\n");
        #endif
        if(percentage > 100)
        {
            percentage = 100;
        }
        char bat[3];
        sprintf(bat,"%d%%",percentage);
        u8g2_DrawBox(&u8g2,0,0,100,32);
        u8g2_DrawBox(&u8g2,105,5,10,22);
        SetDrawColor(0);
        u8g2_DrawBox(&u8g2,3,3,95,26);
        SetDrawColor(1);
        u8g2_DrawBox(&u8g2,4,4,percentage * 0.93,24);
        if(percentage < 40){
            DrawString(40,20,bat);
        }
        else
        {
            SetDrawColor(0);
            if(percentage - 35 >=30)
            {
                percentage = 70;
            }
            DrawString(percentage-35,20,bat);
            SetDrawColor(1);
            
        }
        
        u8g2_SendBuffer(&u8g2);
        
}
