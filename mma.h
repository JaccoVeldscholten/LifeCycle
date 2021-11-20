#ifndef MMA_H
#define MMA_H

/***********************************************************************************/
/*                                Dependencies                                     */
/***********************************************************************************/
#include "mma8x5x.h"
#include "mma8x5x_params.h"
#include "mma8x5x_regs.h"
#include "periph/i2c.h"

/***********************************************************************************/
/*                                  CONSTANTS                                      */
/***********************************************************************************/
/* Motion movement settings */
#define MOTION_G_THRESHOLD 0.8		/* amount of G untill interrupt MOTION is triggered */
#define MOTION_DEBOUNCE 255			/* amount of debounce MOTION timer for how long an interrupt has to occur */
#define MOTION_INTERRUPT_COUNTER 3  /* amount of interrupts before state_alarm is triggered */
#define MOTION_INTERRUPT_RESET true /* if true, interrupt on MOTION will be reset after it is triggered */
#define MOTION_X_ENABLE false		/* if true, enables possibility for MOTION X-axis interrupt */
#define MOTION_Y_ENABLE true		/* if true, enables possibility for MOTION Y-axis interrupt */
#define MOTION_Z_ENABLE false		/* if true, enables possibility for MOTION Z-axis interrupt */

/* Transient movement settings */
#define TRANSIENT_G_THRESHOLD 0.2	   /* amount of G untill interrupt TRANSIENT is triggered */
#define TRANSIENT_DEBOUNCE 2		   /* amount of debounce TRANSIENT timer for how long an interrupt has to occur */
#define TRANSIENT_PID_SLEEPTIME 5      /* amount of seconds TRANSIENT will be shut off */
#define TRANSIENT_INTERRUPT_RESET true /* if true, interrupt on TRANSIENT will be reset after it is triggered */
#define TRANSIENT_X_ENABLE false	   /* if true, enables possibility for TRANSIENT X-axis interrupt */
#define TRANSIENT_Y_ENABLE false	   /* if true, enables possibility for TRANSIENT Y-axis interrupt */
#define TRANSIENT_Z_ENABLE true		   /* if true, enables possibility for TRANSIENT Z-axis interrupt */

/* GPIO settings */
#define GPIO_PIN_MOTION_INTERRUPT (GPIO_PIN(PORT_A, 8))	   /* (D7) set on which pin the MOTION interrupt 1 should happen */
#define GPIO_PIN_TRANSIENT_INTERRUPT (GPIO_PIN(PORT_A, 9)) /* set on which pin the TRANSIENT interrupt 2 should happen */

/* other */
#define MMA_G_STEPSIZE 0.063                    /* The threshold resolution is 0.063 g/LSB and the threshold register has a range of 0 to 127 counts */
                                                /* The maximum range is to 8 g" - https:www.nxp.com/docs/en/data-sheet/MMA8452Q.pdf page 29 */
#define MMA_INTERRUPT_MOTION_CHANNEL 1          /* interrupt number */
#define MMA_INTERRUPT_TRANSIENT_CHANNEL 2       /* interrupt number */
#define MMA_FF_MT_THRESHOLD_CONV 0x7F           /* Treshold for Converting */


/* I2C settings */
#define BUS (dev->params.i2c)   /* bus of MMA i2c */
#define ADDR (dev->params.addr) /* address of MMA i2c */

/***********************************************************************************/
/*                              FUNCTION PROTOTYPES                                */
/***********************************************************************************/
/* Getters/Setters registers*/
void RegClearbits(const mma8x5x_t *dev, uint8_t reg, uint8_t val);
void RegSetbits(const mma8x5x_t *dev, uint8_t reg, uint8_t val);
int RegGetbits(const mma8x5x_t *dev, uint8_t addr);

/* Motion functions*/
void SetMotionDetect(const mma8x5x_t *dev, uint8_t int_pin, uint8_t threshold);
void SetMotionDebounce(const mma8x5x_t *dev, uint8_t val);
void AckMotionInterrupt(const mma8x5x_t *dev);

/* Transient functions*/
void SetTransientDetect(const mma8x5x_t *dev, uint8_t int_pin, uint8_t threshold);
void SetTransientDebounce(const mma8x5x_t *dev, uint8_t val);
void AckTransientInterrupt(const mma8x5x_t *dev);

/* other functions*/
void InitMMA(mma8x5x_t *dev);
int AmountGToMMAValue(double G_input);
void DisableInterrupts(void);
void EnableInterrupts(mma8x5x_t *dev);

#endif