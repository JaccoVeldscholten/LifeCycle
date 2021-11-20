/*! 
 *  \brief     Main file for the GPS driver
 *  \author    Sil Klaasboer
 *  \version   1.0
 *  \date      25-5-2021
 *  \pre       Init usart before calling these functions
 *  \warning   Improper use can crash your application
 *  \copyright GNU LifeCycle.
 */
#include "gps.h"

char temp[LAT_LONG_ARRAY_SIZE];
char last_lat[LAT_LONG_ARRAY_SIZE];
char last_lon[LAT_LONG_ARRAY_SIZE];
char full_coords[LAT_LONG_ARRAY_SIZE];

bool fixAvailable = false;

enum mode
{
	Coords,
	Fix,
	Sat_Inf

} Current_Mode;

char rx_mem[UART_BUFSIZE];
int counter = 0;

int ReturnMode(void)
{
	return Current_Mode;
}
/*!
SetMode() changes the enum currentmode for outputting the correct NMEA GPS message.
*/
void SetMode(uint8_t c)
{
	if (c == 'l')
	{
		Current_Mode = Coords;
		#ifdef DEBUG_GPS
			printf("[GPS] Current_Mode: Coordinates\n");
		#endif
	}
	else if (c == 'f')
	{
		Current_Mode = Fix;
		#ifdef DEBUG_GPS
			printf("[GPS] Current_Mode: Fix_Quality\n");
		#endif
	}
	else if (c == 's')
	{
		Current_Mode = Sat_Inf;
		#ifdef DEBUG_GPS
			printf("[GPS] Current_Mode: Sattalite_Info\n");
		#endif
	}else if( c == 't')
	{
		//Send_PMTK();
	}
}
/*!
Check_Enter() checks the current incoming NMEA messages for enters, if so return true.
*/
int Check_Enter(uint8_t c)
{
	rx_mem[counter++] = c;
    if(c == '\n')
    {
        rx_mem[counter++] = 0;
		counter = 0;
        return true;
    }
	return false;
}

/*!
ReturnLastCoords() returns last known coordinates;
*/
char *ReturnLastCoords(void)
{	
	if((strcmp(last_lon, "") == 0) || (strcmp(last_lat, "") == 0))
	{
		#ifdef DEBUG_GPS
			printf("[GPS] location not in memory sending dummy data\n");
		#endif
		sprintf(full_coords,"%s",DUMMYLOC);
		
	}
	else
	{
		#ifdef DEBUG_GPS
			printf("[GPS] sending last data\n");
		#endif
		sprintf(full_coords,"%s;%s",last_lat,last_lon);
	}
	return(full_coords);
}

/*!
Print_NMEA_Raw() prints the current RAW NMEA message from the gps
*/
void Print_NMEA_Raw(void)
{
	printf("%s", rx_mem);
}

/*!
Print_NMEA_Format() prints the NMEA message from the gps that is formattet and selected bij Current_Mode enum
*/
void Print_NMEA_Format(void)
{
    		if (Current_Mode == Fix)
		{
			if (minmea_sentence_id(rx_mem, false) == MINMEA_SENTENCE_GGA)
			{
				struct minmea_sentence_gga frame;
				minmea_parse_gga(&frame, rx_mem);
					#ifdef DEBUG_GPS
						printf("[GPS] Current_Fix %d\n",frame.fix_quality);
					#endif
			} 

		}

		if (Current_Mode == Coords)
		{
			if (minmea_sentence_id(rx_mem, false) == MINMEA_SENTENCE_RMC)
			{
				struct minmea_sentence_rmc frame;
				minmea_parse_rmc(&frame, rx_mem);
				#ifdef DEBUG_GPS
					printf("[GPS] Current_Location: (%f,%f)\n", minmea_tocoord(&frame.latitude), minmea_tocoord(&frame.longitude));
				#endif
				sprintf(temp, "%f", minmea_tocoord(&frame.latitude));
				if(!(strcmp(temp, "nan") == 0))
				{
					#ifdef DEBUG_GPS
						printf("[GPS] Current_Location parsed to last coordinates\n");
					#endif
					sprintf(last_lat,"%f",minmea_tocoord(&frame.latitude));
					sprintf(last_lon, "%f",minmea_tocoord(&frame.longitude));
				}
			}
		}
		
		if (Current_Mode == Sat_Inf)
		{
			if (minmea_sentence_id(rx_mem, false) == MINMEA_SENTENCE_GSV)
			{
				struct minmea_sentence_gsv frame;
				minmea_parse_gsv(&frame, rx_mem);
				printf("========================================================================\n");
				printf("message %d of %d\n", frame.msg_nr, frame.total_msgs);
				printf("sattelites in view: %d\n", frame.total_sats);
				for (int i = 0; i < 4; i++)
				{
					printf("sat nr %d, elevation: %d, azimuth: %d, snr: %d dbm\n",
						   frame.sats[i].nr,
						   frame.sats[i].elevation,
						   frame.sats[i].azimuth,
						   frame.sats[i].snr);
				}
				printf("========================================================================\n");
			}
		}
}

/*!
Send_PMTK() sending PMTK commando's to GPS to achieve special modi on GPS Module
*/
void Send_PMTK(void)
{	//first parameter = uart dev
	//second parameter = const uint8_t *  	data,
	//last parameter = number of bytes to send
	uart_write(GPS_UART, (uint8_t*)"$PUBX,40,VTG,0,0,0,0*5E\r\n",24);
}
