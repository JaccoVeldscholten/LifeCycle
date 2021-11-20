/***********************************************************************************/
/*                        @@(.                                                     */
/*                       .@&%%%#                         ,#%                       */
/*                           @@                  &@@@@@@@@@&                       */
/*                          @@                       @@                            */
/*                         @@@@@@@@@@@@@@@@@@@@@@@@@@@                             */
/*                       ,@#.@@                   &@,/@@                           */
/*                      &@.   ,@@                @@    @@/                         */
/*          #@@@@@@@@@@@@       ,@@             @@       @@.@@@@@@&@@@@@#          */
/*       @@%          @@ @@@      /@@     .   (@/       .@@@@            @@@       */
/*     @@            @%    .@@      (@&  @   @@        @@   (@%            .@@     */
/*    @@           (@(       @@       (@/    %@      .@%      @@,            @@    */
/*   @@          ,@@          @@     @& @@@@@& @.    @@         @@&,          @%   */
/*   @@         /@@@,         @@     %& @@@@@% @#((((@@(((((((((&@@@          @&   */
/*   (@.                     *@,      .@%    &@      &@                      %@    */
/*    %@.                   *@#            .@         @@.                   %@.    */
/*      @@.               .@@                           @@.               (@@      */
/*        %@@#        .&@@#                               #@@&.       *@@@,        */
/*             /%&&&%*                                         .#%%%/              */
/*                                                                           	   */
/***********************************************************************************/
// 							  LIFECYCLE APPLICATION STM32 
// 								TESTED ON NUCLEO-L476RG
/***********************************************************************************/
/*                                Dependencies                                     */
/***********************************************************************************/
#include <stdio.h>

/* Debug Include */
#include "shell.h"

/* RIOT Includes */
#include "thread.h"
#include "mutex.h"
#include "thread_flags.h"
#include "periph/i2c.h"

/* RIOT OS Includes */
#include "led.h"
#include "board.h"
#include "periph_conf.h"
#include "periph/gpio.h"

/* Hardware Includes */
#include "oled.h"
#include "gps.h"
#include "buzzer.h"
#include "mma.h"
#include "lorawan.h"

#include "Tests/tests.h"


/***********************************************************************************/
/*                                  CONSTANTS                                      */
/***********************************************************************************/
//#define DEBUG									/* Enable to print out information to the Terminal */
#define TEST									/* Enable to run Unit Tests */
	
#define RCV_QUEUE_SIZE 8						/* Queue Size for RCV */
#define mSECONDS_FOR_EMERGENCY_BUTT 2000	
#define mSECONDS_FOR_ALARM_STOP 1

#define GPIO_PIN_START_ALARM_BTN (GPIO_PIN(PORT_C, 3)) /* set on wich pin the alarm should start */
#define GPIO_PIN_STOP_ALARM_BTN (GPIO_PIN(PORT_C, 2))  /* set on which pin the alarm should stop */

/* Device States */
typedef enum
{
	Sleep,
	Normal,
	BackToNormal,
	Alarm,
} State_t;


//PIDs to control the threads
static kernel_pid_t printer_pid;
static kernel_pid_t battery_pid;
static kernel_pid_t transient_pid;
static kernel_pid_t loraSender_pid;
static kernel_pid_t main_pid;

/* Reserved Stacks for threads */
static char main_stack[THREAD_STACKSIZE_MAIN ];
static char transient_stack[THREAD_STACKSIZE_MAIN];
static char battery_stack[THREAD_STACKSIZE_MAIN];
static char sender_stack[THREAD_STACKSIZE_MAIN ];
#ifdef DEBUG
static char printer_stack[THREAD_STACKSIZE_MAIN];
#endif
/***********************************************************************************/
/*                                  Temp Vars                                      */
/***********************************************************************************/
static mma8x5x_t mmaDevice;

static msg_t loraMsg;
static msg_t rcv_queue[RCV_QUEUE_SIZE];
static State_t state;

uint8_t transientPidCounter;
uint8_t mmaMotionCounter;

bool DisplayActive;
bool TurnedOffOled;								
bool LoRaAlarmSend;								/* Boolean to prevent from continous sending to keep Duty Cycle Limits */
bool ButtonPressed;


/***********************************************************************************/
/*                    IsEmergencyButtonPressed function				       		   */
/*       Function for watching and acting upon emergency button pressed		 	   */
/***********************************************************************************/
static void IsEmergencyButtonPressed(void)
{
	static uint8_t counter = 1;
	if (gpio_read(GPIO_PIN_START_ALARM_BTN) == 0) /* see if the button is pressed */
	{
		if (counter * 1000 > (mSECONDS_FOR_EMERGENCY_BUTT))  /*  see if the counter is equal to the time it should take to press the button */
		{
#ifdef DEBUG
			puts("IsEmergencyButtonPressed: button held for long enough"); // for debug
#endif
			thread_wakeup(main_pid); 					/* Starting the Main Thread */
			counter = 1;		/* set the counter back to 1 */
			state = Alarm; 		/* Change state to alarm mode */
		}
		else
		{ 
			counter++;
#ifdef DEBUG
			//puts("IsEmergencyButtonPressed: Counter++"); // for debug
#endif
		}
	}
	else
	{
		counter = 1;
#ifdef DEBUG
		//puts("IsEmergencyButtonPressed: button not held for long enough"); // for debug
#endif
	}
}

/***********************************************************************************/
/*                    IsStopAlarmButtonPressed function				       		   */
/*       Function for watching and acting upon stop alarm button pressed	 	   */
/***********************************************************************************/
static void IsStopAlarmButtonPressed(void)
{
	static uint8_t counter = 1;
	if (gpio_read(GPIO_PIN_STOP_ALARM_BTN) == 0)
	{
		if (counter > mSECONDS_FOR_ALARM_STOP)
		{
#ifdef DEBUG
			printf("IsStopAlarmButtonPressed: button held for long enough"); // for debug
#endif		
			
			counter = 1;
			state = BackToNormal; /* Change device mode to Semi Normal, Main Thread will handle this */
		}
		else
		{
			counter++;
#ifdef DEBUG
			//puts("IsStopAlarmButtonPressed: Counter++"); // for debug
#endif
		}
	}
	else
	{
		counter = 1;
#ifdef DEBUG
		//puts("IsStopAlarmButtonPressed: button not held for long enough"); // for debug
#endif
	}
}

/***********************************************************************************/
/*                             	 CB_Motion                              		   */
/* 	       When Fall occurs this function will change the state of the device      */
/***********************************************************************************/
static void CB_Motion(void *arg)
  /* when Fall occurs (wakeup) */
{
	(void)arg;
	#ifdef DEBUG
		printf("[MMA] MOTION Interrupt\n");
	#endif
	msg_init_queue(rcv_queue, RCV_QUEUE_SIZE);

	mmaMotionCounter++;
	if (mmaMotionCounter == MOTION_INTERRUPT_COUNTER)
	{
		state = Alarm; /* Change the device state to alarm mode */
	}

	if (MOTION_INTERRUPT_RESET)
	{
		AckMotionInterrupt(&mmaDevice);
	}
}

/***********************************************************************************/
/*                             	 CB_Transient                              		   */
/* 	  When Movement occurs this function will change the state of the device      */
/***********************************************************************************/
void CB_Transient(void *arg)
{
	(void)arg;
	#ifdef DEBUG
		printf("[MMA] TRANSIENT Interrupt\n");
	#endif
	if(state != Alarm)
	{
		state = Normal; /* Change State to Normal Mode */
	}
	thread_wakeup(transient_pid);
}
/***********************************************************************************/
/*                              CBBatteryButtonRising                              */
/*                   Handles the Interrupt for the Emergency Button                */
/***********************************************************************************/
static void CBBatteryButtonRising(void *arg)
{
	(void)arg;
	if (state != Alarm){
		DisplayActive = !DisplayActive;
	}
}

/***********************************************************************************/
/*                      Read emergency button Callback function				       */
/*       interrupt for handling the emergency button and changing the state		   */
/*								\author Sil Klaasboer							   */
/***********************************************************************************/
static void CB_EmergencyButt(void *arg)
{
	(void)arg;
	printf("\n###########    CB_EmergencyButt on interrupt!    ###########\n");
	thread_wakeup(main_pid);
	IsEmergencyButtonPressed();
}

/***********************************************************************************/
/*                   Read stop alarm button Callback function				       */
/*       interrupt for handling the emergency button and changing the state		   */
/***********************************************************************************/
static void CB_StopAlarmButton(void *arg)
{
	(void)arg;
	printf("\n###########    CB_StopAlarmButton interrupt!    ###########\n");
	thread_wakeup(main_pid);		/* Always call the Main Thread */
	IsStopAlarmButtonPressed();
}

/***********************************************************************************/
/*                           Read_SHELL Callback function				       	   */
/*   interrupt for incoming data from the Desktop to set diffrent GPS output mode  */
/***********************************************************************************/

static void Read_SHELL(void *uart, uint8_t c)
{
	(void)uart;
	SetMode(c);
}

/***********************************************************************************/
/*                           Read_GPS Callback function				       		   */
/*       interrupt for incoming data from the GPS to send message to printer 	   */
/***********************************************************************************/

static void Read_GPS(void *uart, uint8_t c)
{
	(void)uart;
	if (Check_Enter(c))
	{
		msg_t msg;
		msg_send(&msg, printer_pid);
	}
}

/***********************************************************************************/
/*                               	 Printer Thread				                   */
/*      	 for printing the GPS debug information to the uart bus 			   */
/***********************************************************************************/
#ifdef DEBUG
void *printer(void *arg)
{
	(void)arg;
	msg_t msg;
	msg_t msg_queue[8];
	msg_init_queue(msg_queue, 8);
	for (;;)
	{
		msg_receive(&msg);
		Print_NMEA_Format();
		xtimer_msleep(100);
	}
	return 0;
}
#endif
/***********************************************************************************/
/*                           LoRaWan Sender Thread	                               */
/*      	 Used to send information to the Cloud by Message Queue & Interrupt    */
/*           LoRaWan uses an Queue to send information when the thread is ready    */
/***********************************************************************************/
static void *LoraSender(void *arg)
{
	(void)arg;

	msg_t loraMsg;										
	msg_t msg_queue[LORA_QUEUE];
	msg_init_queue(msg_queue, LORA_QUEUE);

	while (1)
	{
		msg_receive(&loraMsg);
		if (SendMsg()) 				/* Trigger the message send to attempt sending */
		{
			#ifdef DEBUG
				printf("[OLED] Lora send succes\n");
			#endif
			DisableInterrupts();
			SetPowerSave(0);
			SetFont(u8g2_font_crox3tb_tr);
			Draw1Line(0, 20, "MSG SEND");
		}
		else
		{
			#ifdef DEBUG
				printf("[OLED] Lora send fail\n");
			#endif
			DisableInterrupts();
			SetPowerSave(0);
			SetFont(u8g2_font_crox3tb_tr);
			Draw1Line(0, 20, "MSG FAIL");
		}
	}
	return 0;
}
/***********************************************************************************/
/*                              Battery Sender Thread                              */
/*      Retreives ADC Value of the battery connected Port. Send Battery by LoRa    */
/*   Currently the ADC is not supported and the function will always send out 44%  */
/***********************************************************************************/

static void *BatteryUpdater(void *arg)
{
    (void)arg;

    while (1) {
		#ifdef DEBUG
			printf("[LORA] Battery UpDate \n");
		#endif
		LoraEnqueueBattery(44);      /* Trigger the message send */ 
		#ifdef DEBUG
			xtimer_sleep(10);			/* Sleep for 10 Seconds for Debug.*/
		#else
			xtimer_sleep(3600);			/* Sleep for 3600 Seconds for general Purpose.*/
		#endif
	}
	return 0;
}

/***********************************************************************************/
/*                              Transient Thread                                   */
/*      puts Transient mode into sleep, then resets so it can interrupt again      */
/***********************************************************************************/

static void *Transientpid(void *arg)
{
	(void)arg;

	while (1)
	{
		#ifdef DEBUG
			printf("[MMA] Transient read\n");
		#endif
		thread_wakeup(main_pid); 					/* Starting the Main Thread */
		transientPidCounter++;
		if (transientPidCounter >= TRANSIENT_PID_SLEEPTIME)
		{
			if (TRANSIENT_INTERRUPT_RESET)
			{
				transientPidCounter = 0;
				AckTransientInterrupt(&mmaDevice);
				#ifdef DEBUG
					printf("[MMA] Transient can interrupt again\n");
					printf("[MMA] Transient pid sleep\n");
				#endif
				thread_sleep();
			}
		}
		xtimer_msleep(1000);
	}
	return 0;
}

/***********************************************************************************/
/*                          		Main Thread 	                               */
/*      	 Used to send information to the Cloud by Message Queue & Interrupt    */
/***********************************************************************************/
static void *mainThread(void *arg)
{
    (void)arg;

    while (1) {
		switch (state){
            case Normal:
					#ifdef DEBUG
						printf("[MAIN] Loop in Normal state\n");
					#endif
				if(DisplayActive)
                {
                        DisableInterrupts();
                        SetPowerSave(0);
                        SetFont(u8g2_font_crox3tb_tr);
                        DrawBattery(30);
                        EnableInterrupts(&mmaDevice);
                        TurnedOffOled = false;
                }
                else
                {
                    if(!DisplayActive && !TurnedOffOled)
                    {   
                        DisableInterrupts();
                        ClearDisplay();
                        SetPowerSave(1);
                        EnableInterrupts(&mmaDevice);
                        TurnedOffOled = true;
                    }
                }
			    break;
		case Alarm:
			#ifdef DEBUG
			printf("Alarm state started \n");
			#endif
			if (LoRaAlarmSend)
			{
				/* PWM Buzzer */
				CycleBetweenFrequencies(3000, 5000, 100, 200, 50, 100);
				SOS();
				CycleBetweenFrequencies(5000, 3000, 100, 200, 50, 100);
				DisableInterrupts();
				SetPowerSave(0);
				SetFont(u8g2_font_crox3tb_tr);
				Draw1Line(0, 20, "ALARM!!");
			}
			else
			{
				/* Disable the IRQ From MMA so sending the alarm will not be interfered */
				DisableInterrupts();
				LoraEnqueueAlarm(ReturnLastCoords());
				//loraSendAlarm(ReturnLastCoords); ToDo: What if there are no coords?
				msg_send(&loraMsg, loraSender_pid);
				LoRaAlarmSend = true; /* Stop Lora from sending the alarm continous */
			}
			break;
		case BackToNormal:
			/* Change the device back to normal mode, bring back device in sleep mode */
			TurnBuzzerOff();
			ClearDisplay();
			LoRaAlarmSend = false;
			LoraEnqeueCancelAlarm();					
			msg_send(&loraMsg,loraSender_pid);		 /* Call MSG to send Cancel Alarm Message */
			state = Normal; 						 /* Change state back to normal */
			thread_sleep();					 		 /* Let the Main Thread sleep */
			break;	
		default:
			break;
		}
		IsEmergencyButtonPressed();
		IsStopAlarmButtonPressed();
		xtimer_msleep(1000);
	}
	return 0;
}

/***********************************************************************************/
/*                               	 Main				                           */
/*      	for creating the threads and initializing certain functions			   */
/***********************************************************************************/
int main(void)
{
#if defined TEST || defined DEBUG
	uart_init(PC_UART, BAUDRATE, Read_SHELL, (void *)PC_UART);	/* Set the correct Baud to prevent interference with GPS */
#endif
	uart_init(GPS_UART, BAUDRATE, Read_GPS, (void *)GPS_UART);

#ifdef TEST
	TestSuite();		/* Calling the testing enviroment */
	exit(-1);			/* In Case of testing, close the application afterwards */
#endif

  	/* Initialize Global Variables */
	mmaMotionCounter = 0;                   /* Motion counter must be interrupted 3 times before switching state */
	transientPidCounter = 0;
	state = Normal;                         /* Starting the device at Normal mode */
	DisplayActive = false;
 	TurnedOffOled = true;
 	LoRaAlarmSend = false;
 	ButtonPressed = true;

#ifdef DEBUG
		printf("[MAIN] Uart Init\n");
#endif

#ifdef DEBUG
		printf("[MAIN] GPIO Init\n");
#endif
	gpio_init_int(GPIO_PIN_MOTION_INTERRUPT, GPIO_IN, GPIO_FALLING, CB_Motion, NULL);			/* interrupt 1 - FF_MT */
	gpio_init_int(GPIO_PIN_TRANSIENT_INTERRUPT, GPIO_IN, GPIO_FALLING, CB_Transient, NULL);		/* interrupt 2 - transient */
	gpio_init_int(GPIO_PIN_START_ALARM_BTN, GPIO_IN_PU, GPIO_FALLING, CB_EmergencyButt, NULL);	/* button to start alarm interrupt by Sjoerd */
	gpio_init_int(GPIO_PIN_STOP_ALARM_BTN, GPIO_IN_PU, GPIO_FALLING, CB_StopAlarmButton, NULL); /* button to stop alarm interrupt by Sjoerd */
	gpio_init_int(BTN0_PIN, BTN0_MODE, GPIO_RISING, CBBatteryButtonRising, NULL);

	InitLoRaWan();							/* initializes LoRa & Join Network  */
	InitI2cDisplay();						/* initializes OLED 				*/
	InitBuzzer();							/* initializes Alarm Buzzer			*/
	InitMMA(&mmaDevice);					/* initializes MMA 					*/
	

/*
Threads & Callback				                          
The following threads will be created and starting in diffrent modes	 
+-----------+----------+-------------+---------------+----------------------------------------------------------------------------------------------------------+
| Thread    | Type     | Prio        | Starting Mode | Description                                                                                              |
+-----------+----------+-------------+---------------+----------------------------------------------------------------------------------------------------------+
| Main      | Thread   | 1 (Highest) | Sleeping      | The Main thread for the whole application. Will be activated by interrupt of the MMA Transient           |
+-----------+----------+-------------+---------------+----------------------------------------------------------------------------------------------------------+
| Lora      | CallBack | 2           | Idle / Empty  | Sends out information when the LoRa Queue is filled and Callback is triggerd                             |
+-----------+----------+-------------+---------------+----------------------------------------------------------------------------------------------------------+
| Transient | Thread   | 3           | Sleeping      | Starts counting then interrupted by the MMA Transient detection                                          |
+-----------+----------+-------------+---------------+----------------------------------------------------------------------------------------------------------+
| Battery   | Thread   | 4           | Active        | Updates the battery every x second                                                                       |
+-----------+----------+-------------+---------------+----------------------------------------------------------------------------------------------------------+
| Printer   | Callback | 5 (Lowest)  | Idle / Empty  | Sends out information when the UART Queue is filled and Callback is triggerd. Only enabled in DEBUG Mode |
+-----------+----------+-------------+---------------+----------------------------------------------------------------------------------------------------------+
*/

/* Call Back Functions */

	loraSender_pid = thread_create(sender_stack, sizeof(sender_stack),
						THREAD_PRIORITY_MAIN - 4, // Prio 2
						0,
						LoraSender,
						NULL, "loraSender");

#ifdef DEBUG
	printer_pid = thread_create(printer_stack, sizeof(printer_stack),
						THREAD_PRIORITY_MAIN - 1, // Prio 5
						0,
						printer,
						NULL, "uartPrinter");
#endif

 /* Theads */

 	main_pid = thread_create(main_stack, sizeof(main_stack),
						THREAD_PRIORITY_MAIN - 5, // Prio 1
						THREAD_CREATE_SLEEPING,
						mainThread,
						NULL, "mainpid");

	transient_pid = thread_create(transient_stack, sizeof(transient_stack),
						THREAD_PRIORITY_MAIN - 3, // Prio 3
						THREAD_CREATE_SLEEPING,
						Transientpid,
						NULL, "transientpid");
	battery_pid = thread_create(battery_stack, sizeof(battery_stack),
						THREAD_PRIORITY_MAIN - 2, // Prio 4
						0,
						BatteryUpdater,
						NULL, "batteryUpdate");

	return 0;		/* Should never reached */
}
