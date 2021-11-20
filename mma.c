#include "mma.h"

/*!
InitMMA() initialize MMA with all prefs
*/
void InitMMA(mma8x5x_t *dev) 
{
	printf("[MMA] Initializing MMA8452 accelerometer at I2C_DEV(%i)... ", mma8x5x_params->i2c);
	/* checks that mma is connected */
	if (mma8x5x_init(dev, mma8x5x_params) == MMA8X5X_OK)
	{
		puts("[OK]\n");

		/* enable motion / transient detection */
		SetMotionDetect(dev, MMA_INTERRUPT_MOTION_CHANNEL, AmountGToMMAValue(MOTION_G_THRESHOLD));
		SetTransientDetect(dev, MMA_INTERRUPT_TRANSIENT_CHANNEL, AmountGToMMAValue(TRANSIENT_G_THRESHOLD));

		/* set debounce timer for how long G-threshold has to be overflown */
		SetMotionDebounce(dev, MOTION_DEBOUNCE);
		SetTransientDebounce(dev, TRANSIENT_DEBOUNCE);
	}
	else
	{
		puts("[Failed]");
		/* return -1; // stop the program from starting */
	}
}

/*!
RegClearbits() Clear bits in the given mma8x5x registers
*/
void RegClearbits(const mma8x5x_t *dev, uint8_t reg, uint8_t val)
{
	uint8_t tmp;
	/* make i2c connection */
	i2c_acquire(BUS);
	i2c_read_reg(BUS, ADDR, reg, &tmp, 0);

	/* clear given bits */
	tmp &= ~val;
	i2c_write_reg(BUS, ADDR, reg, tmp, 0);
	i2c_release(BUS);
}

/*!
RegSetbits() Set bits in the given mma8x5x registers
*/
void RegSetbits(const mma8x5x_t *dev, uint8_t reg, uint8_t val)
{
	uint8_t tmp;
	/* make i2c connection */
	i2c_acquire(BUS);
	i2c_read_reg(BUS, ADDR, reg, &tmp, 0);
	/* set given bits */
	tmp |= val;
	i2c_write_reg(BUS, ADDR, reg, tmp, 0);
	i2c_release(BUS);
}

/*!
RegGetbits() Get bits in the given mma8x5x registers
*/
int RegGetbits(const mma8x5x_t *dev, uint8_t addr)
{
	uint8_t reg;
	/* make i2c connection */
	i2c_acquire(BUS);
	/* get register */
	i2c_read_reg(BUS, ADDR, addr, &reg, 0);
	i2c_release(BUS);

	return reg;
}

/* -------------------   Motion functions   ------------------- */

/*!
SetMotionDetect() Turn on Motion Detection in the mma8x5x
*/
void SetMotionDetect(const mma8x5x_t *dev, uint8_t int_pin, uint8_t threshold)
{
	/* temp turn off MMA */
	mma8x5x_set_standby(dev);
	/* set motion detection on */
	RegSetbits(dev, MMA8X5X_FF_MT_CFG,
				 MMA8X5X_FF_MT_CFG_OAE | MMA8X5X_FF_MT_CFG_ELE);

	/* set which axes should motion detect */
	if (MOTION_X_ENABLE)
	{
		RegSetbits(dev, MMA8X5X_FF_MT_CFG, MMA8X5X_FF_MT_CFG_XEFE);
	}

	if (MOTION_Y_ENABLE)
	{
		RegSetbits(dev, MMA8X5X_FF_MT_CFG, MMA8X5X_FF_MT_CFG_YEFE);
	}

	if (MOTION_Z_ENABLE)
	{
		RegSetbits(dev, MMA8X5X_FF_MT_CFG, MMA8X5X_FF_MT_CFG_ZEFE);
	}

	/* set which output interrupt pin should trigger on motion */
	switch (int_pin)
	{
	case 0:
		RegClearbits(dev, MMA8X5X_CTRL_REG4, MMA8X5X_CTRL_REG4_INT_EN_FF_MT);
		goto out;
	case 1:
		RegSetbits(dev, MMA8X5X_CTRL_REG5, MMA8X5X_CTRL_REG5_INT_CFG_FF_MT);
		break;
	case 2:
		RegClearbits(dev, MMA8X5X_CTRL_REG5, MMA8X5X_CTRL_REG5_INT_CFG_FF_MT);
		break;
	}

	RegSetbits(dev, MMA8X5X_FF_MT_THS, threshold & 0x7f);
	RegSetbits(dev, MMA8X5X_CTRL_REG4, MMA8X5X_CTRL_REG4_INT_EN_FF_MT);

out:
	mma8x5x_set_active(dev);
}


/*!
DisableInterrupts() Disables Motion- and Transient Interrupt
*/
void DisableInterrupts(void)
{
	/* disables interrupt on Motion and Transient */
    gpio_irq_disable(GPIO_PIN_MOTION_INTERRUPT);
    gpio_irq_disable(GPIO_PIN_TRANSIENT_INTERRUPT);
}

/*!
EnableInterrupts() Enables Motion- and Transient Interrupt
*/
void EnableInterrupts(mma8x5x_t *dev)
{
	/* enable interrupt on Motion and Transient */
    gpio_irq_enable(GPIO_PIN_MOTION_INTERRUPT); 
    gpio_irq_enable(GPIO_PIN_TRANSIENT_INTERRUPT);
	/* reset interrupts on Motion and Transient (so they can interrupt again) */
    AckMotionInterrupt(dev);
    AckTransientInterrupt(dev);
}

/*!
SetMotionDebounce() Set amount of time that force has to be messured before interrupt triggers
*/
void SetMotionDebounce(const mma8x5x_t *dev, uint8_t val)
{
	RegSetbits(dev, MMA8X5X_FF_MT_COUNT, val);
}

/*!
AckMotionInterrupt() Acknowledge Interrupt so new Interrupt can happen (reset interrupt flag)
*/
void AckMotionInterrupt(const mma8x5x_t *dev)
{
	RegGetbits(dev, MMA8X5X_FF_MT_SRC);
}

/* -------------------   Transient functions   ------------------- */

/*!
SetTransientDetect() Turn on Transient Detection in the mma8x5x
*/
void SetTransientDetect(const mma8x5x_t *dev, uint8_t int_pin, uint8_t threshold)
{
	/* temp turn off MMA */
	mma8x5x_set_standby(dev);

	/* set transient detection on */
	RegSetbits(dev, MMA8X5X_TRANSIENT_CFG, //MMA8X5X_TRANSIENT_CFG_HPF_BYP |
				 MMA8X5X_TRANSIENT_CFG_ELE);
	
	/* set which axes should motion detect */
	if (TRANSIENT_X_ENABLE)
	{
		RegSetbits(dev, MMA8X5X_TRANSIENT_CFG, MMA8X5X_TRANSIENT_CFG_XTEFE);
	}

	if (TRANSIENT_Y_ENABLE)
	{
		RegSetbits(dev, MMA8X5X_TRANSIENT_CFG, MMA8X5X_TRANSIENT_CFG_YTEFE);
	}

	if (TRANSIENT_Z_ENABLE)
	{
		RegSetbits(dev, MMA8X5X_TRANSIENT_CFG, MMA8X5X_TRANSIENT_CFG_ZTEFE);
	}

	/* set which output interrupt pin should trigger on motion */
	switch (int_pin)
	{
	case 0:
		RegClearbits(dev, MMA8X5X_CTRL_REG4, MMA8X5X_CTRL_REG4_INT_EN_TRANS);
		goto out;
	case 1:
		RegSetbits(dev, MMA8X5X_CTRL_REG5, MMA8X5X_CTRL_REG5_INT_CFG_TRANS);
		break;
	case 2:
		RegClearbits(dev, MMA8X5X_CTRL_REG5, MMA8X5X_CTRL_REG5_INT_CFG_TRANS);
		break;
	}

	RegSetbits(dev, MMA8X5X_TRANSIENT_THS, threshold & 0x7f);
	RegSetbits(dev, MMA8X5X_CTRL_REG4, MMA8X5X_CTRL_REG4_INT_EN_TRANS);

out:
	mma8x5x_set_active(dev);
}

/*!
SetTransientDebounce() Set amount of time that force has to be messured before interrupt triggers
*/
void SetTransientDebounce(const mma8x5x_t *dev, uint8_t val)
{
	RegSetbits(dev, MMA8X5X_TRANSIENT_COUNT, val);
}

/*!
AckTransientInterrupt() Acknowledge Interrupt so new Interrupt can happen (reset interrupt flag)
*/
void AckTransientInterrupt(const mma8x5x_t *dev)
{
	RegGetbits(dev, MMA8X5X_TRANSIENT_SRC);
}

/* -------------------   Other functions   ------------------- */

/*!
AmountGToMMAValue() converts amount of G-input to stepsize
*/
int AmountGToMMAValue(double G_input)
{
	/* make amount of G input to mma-stepsize in register */
	int MMA_Value = (int)(G_input / MMA_G_STEPSIZE);
	return MMA_Value;
}
