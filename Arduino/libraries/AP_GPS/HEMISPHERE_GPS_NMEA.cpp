//// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: t -*-
/*
	GPS_NMEA.cpp - Generic NMEA GPS library for Arduino
	Code by Jordi Mu�oz and Jose Julio. DIYDrones.com
	Edits by HappyKillmore
	This code works with boards based on ATMega168 / 328 and ATMega1280 (Serial port 1)

	This library is free software; you can redistribute it and / or
		modify it under the terms of the GNU Lesser General Public
		License as published by the Free Software Foundation; either
		version 2.1 of the License, or (at your option) any later version.

	GPS configuration : NMEA protocol
	Baud rate : 38400
	NMEA Sentences : 
		$GPGGA : Global Positioning System fix Data
		$GPVTG : Ttack and Ground Speed
		
	Methods:
		init() : GPS Initialization
		read() : Call this funcion as often as you want to ensure you read the incomming gps data
		
	Properties:
		latitude : latitude * 10000000 (long value)
		longitude : longitude * 10000000 (long value)
		altitude :	altitude * 1000 (milimeters) (long value)
		ground_speed : Speed (m / s) * 100 (long value)
		ground_course : Course (degrees) * 100 (long value)
		Type : 2 (This indicate that we are using the Generic NMEA library)
		new_data : 1 when a new data is received.
							You need to write a 0 to new_data when you read the data
		fix : > = 1: GPS FIX, 0: No fix (normal logic)
		quality : 0 = No fix
							 1 = Bad (Num sats < 5)
					 2 = Poor
					 3 = Medium
					 4 = Good

	 NOTE : This code has been tested on a Locosys 20031 GPS receiver (MTK chipset)
*/
#include "HEMISPHERE_GPS_NMEA.h"
	



HEMISPHERE_GPS_NMEA::HEMISPHERE_GPS_NMEA(Stream *s) : GPS(s)
{
}

// Public Methods //////////////////////////////////////////////////////////////
void 
HEMISPHERE_GPS_NMEA::init(void)
{
	/*
	//Type = 2;
	// initialize serial port for binary protocol use
	_port->print(HEMISPHERE_BAUD_RATE_A_19200); // Set Baud Rate 19200 for current port A
    _port->print(HEMISPHERE_PORT_A_DATA_OFF);	// Turn all Messages to OFF
    _port->print(HEMISPHERE_GP_HEADING);		// Set GPHDT/HEHDT to use GPHDT
	_port->print(HEMISPHERE_OUTPUT_GGA);		// GPGGA  Message request
	_port->print(HEMISPHERE_OUTPUT_VTG);		// GPVTG  Message request
	_port->print(HEMISPHERE_OUTPUT_HDT);		// GPHDT  Message request
	_port->print(HEMISPHERE_PORT_C_SERIAL);		// Set Port C to Srial
	_port->print(HEMISPHERE_BAUD_RATE_C_19200); // Set Baud Rate 19200 for port C
	//_port->print(HEMISPHERE_SHOW);                 
  */	
	
}

// This code don�t wait for data, only proccess the data available on serial port
// We can call this function on the main loop (50Hz loop)
// If we get a complete packet this function call parse_nmea_gps() to parse and update the GPS info.

/*
void HEMISPHERE_GPS_NMEA::showStatus(void)
{
	  
    
	char c;
	int numc;
	int i;    
    
    _port->print(HEMISPHERE_SHOW);    numc = _port->available(); 
    
    
	
	numcGlobal = numc;                //JK added **********************************************
	showSentence[0] = '\0';               //JK added **********************************************
    
	if (numc > 0){
		for (i = 0; i < numc; i++){
			c = _port->read();
            
            if (i < 1200) {              //JK added ********************************************** 
                showSentence[i] = c;        //JK added **********************************************
                showSentence[i + 1] = '\0'; //JK added **********************************************           
            }
        }
    }
    
}
 */
 
 
bool
HEMISPHERE_GPS_NMEA::read(void)
{
	char c;
	int numc;
	int i;
	bool parsed = false;
    

    
 	numc = _port->available();
	
	//numcGlobal = numc;                //JK added **********************************************
	//sentence[0] = '\0';               //JK added **********************************************
    
	if (numc > 0){
		for (i = 0; i < numc; i++){
			c = _port->read();
            
          //  if (i < 1200) {              //JK added ********************************************** 
          //      sentence[i] = c;        //JK added **********************************************
          //      sentence[i + 1] = '\0'; //JK added **********************************************           
          //  }
                                       
			if (c == '$'){											// NMEA Start
				bufferidx = 0;
				buffer[bufferidx++] = c;
				GPS_checksum = 0;
				GPS_checksum_calc = true;
				continue;
				}
			if (c == '\r'){										 // NMEA End
				
                
				buffer[bufferidx++] = 0;
				

				
				
				parsed = parse_nmea_gps();
			} else {
				if (bufferidx < (GPS_BUFFERSIZE - 1)){
					if (c == '*')
						GPS_checksum_calc = false;		// Checksum calculation end
					buffer[bufferidx++] = c;
					if (GPS_checksum_calc){
						GPS_checksum ^= c;						// XOR 
					}
				} else {
					bufferidx = 0;	 // Buffer overflow : restart
				}
			}
		}
	}
}

/****************************************************************
 * 
 * * ** * ** * ** * ** * ** * ** * ** * ** * ** * ** * ** * ** * ** * ** * ** * ** * ** * ** * ** * ** * **/
// Private Methods //////////////////////////////////////////////////////////////
bool
HEMISPHERE_GPS_NMEA::parse_nmea_gps(void)
{
	uint8_t NMEA_check;
	long aux_deg;
	long aux_min;
	char *parseptr;
	


	
	if (strncmp(buffer,"$GPGGA",6)==0){					// Check if sentence begins with $GPGGA
		if (buffer[bufferidx-4]=='*'){					 // Check for the "*" character
			NMEA_check = parseHex(buffer[bufferidx - 3]) * 16 + parseHex(buffer[bufferidx - 2]);		// Read the checksums characters
			if (GPS_checksum == NMEA_check){			// Checksum validation
				//Serial.println(buffer);
				parseptr = strchr(buffer, ',')+1;
				//parseptr = strchr(parseptr, ',')+1;
				time = parsenumber(parseptr, 2);					// GPS UTC time hhmmss.ss
				parseptr = strchr(parseptr, ',')+1;
				aux_deg = parsedecimal(parseptr, 2);			// degrees
				aux_min = parsenumber(parseptr + 2, 5);		 // minutes (sexagesimal) => Convert to decimal
				latitude = aux_deg * 10000000 + (aux_min * 50) / 30;	 // degrees + minutes / 0.6	( * 10000000) (0.6 = 3 / 5)
				parseptr = strchr(parseptr, ',')+1;
				if ( * parseptr == 'S')
					latitude = -1 * latitude;							// South latitudes are negative
				parseptr = strchr(parseptr, ',')+1;
				// W longitudes are Negative
				aux_deg = parsedecimal(parseptr, 3);			// degrees
				aux_min = parsenumber(parseptr + 3, 5);		 // minutes (sexagesimal)
				longitude = aux_deg * 10000000 + (aux_min * 50) / 30;	// degrees + minutes / 0.6 ( * 10000000)
				//longitude = -1*longitude;									 // This Assumes that we are in W longitudes...
				parseptr = strchr(parseptr, ',')+1;
				if ( * parseptr == 'W')
					longitude = -1 * longitude;							// West longitudes are negative
				parseptr = strchr(parseptr, ',')+1;
				fix = parsedecimal(parseptr, 1);
				hemisphereQuality = fix;						//jk added ****************************************				
				parseptr = strchr(parseptr, ',')+1;
				num_sats = parsedecimal(parseptr, 2);
				hemisphereSatelites = num_sats;
				parseptr = strchr(parseptr, ',')+1; 
				hdop = parsenumber(parseptr, 1);					// HDOP * 10
				parseptr = strchr(parseptr, ',')+1;
				altitude = parsenumber(parseptr, 1) * 100;	// altitude in decimeters * 100 = milimeters
				if (fix < 1)
					quality = 0;			// No FIX
				else if(num_sats < 5)
					quality = 1;			// Bad (Num sats < 5)
				else if(hdop > 30)
					quality = 2;			// Poor (HDOP > 30)
				else if(hdop > 25)
					quality = 3;			// Medium (HDOP > 25)
				else
					quality = 4;			// Good (HDOP < 25)
			} else {
				_error("GPSERR: Checksum error!!\n");
				return false;
			}
		}
	} else if (strncmp(buffer,"$GPVTG",6)==0){				// Check if sentence begins with $GPVTG
		//Serial.println(buffer);
		if (buffer[bufferidx-4]=='*'){					 // Check for the "*" character
			NMEA_check = parseHex(buffer[bufferidx - 3]) * 16 + parseHex(buffer[bufferidx - 2]);		// Read the checksums characters
			if (GPS_checksum == NMEA_check){			// Checksum validation
				parseptr = strchr(buffer, ',')+1;
				ground_course = parsenumber(parseptr, 1) * 10;			// Ground course in degrees * 100
				parseptr = strchr(parseptr, ',')+1;
				parseptr = strchr(parseptr, ',')+1;
				parseptr = strchr(parseptr, ',')+1;
				parseptr = strchr(parseptr, ',')+1;
				parseptr = strchr(parseptr, ',')+1;
				parseptr = strchr(parseptr, ',')+1;
				ground_speed = parsenumber(parseptr, 1) * 100 / 36; // Convert Km / h to m / s ( * 100)
				//GPS_line = true;
			}
		}
    } else if (strncmp(buffer,"$GPHDT",6)==0){				// Check if sentence begins with $GPHTD
        //Serial.println(buffer);
        if (buffer[bufferidx-4]=='*'){					 // Check for the "*" character
            NMEA_check = parseHex(buffer[bufferidx - 3]) * 16 + parseHex(buffer[bufferidx - 2]);		// Read the checksums characters
            if (GPS_checksum == NMEA_check){			// Checksum validation
                parseptr = strchr(buffer, ',')+1;
				true_heading = parsenumber(parseptr, 1) * 10;			// true heading in degrees * 100
                
			} else {
				_error("GPSERR: Checksum error!!\n");
				return false;
			}
		}
	
	} else {
		bufferidx = 0;
		_error("GPSERR: Bad sentence!!\n");
		return false;
	}
	return true;
}


 // Parse hexadecimal numbers
uint8_t
HEMISPHERE_GPS_NMEA::parseHex(char c) {
		if (c < '0')
			return (0);
		if (c <= '9')
			return (c - '0');
		if (c < 'A')
			 return (0);
		if (c <= 'F')
			 return ((c - 'A')+10);
}

// Decimal number parser
long
HEMISPHERE_GPS_NMEA::parsedecimal(char *str, uint8_t num_car) {
	long d = 0;
	uint8_t i;
	
	i = num_car;
	while ((str[0] != 0) && (i > 0)) {
		if ((str[0] > '9') || (str[0] < '0'))
			return d;
		d *= 10;
		d += str[0] - '0';
		str++;
		i--;
	}
	return d;
}

// Function to parse fixed point numbers (numdec=number of decimals)
long
HEMISPHERE_GPS_NMEA::parsenumber(char *str, uint8_t numdec) {
	long d = 0;
	uint8_t ndec = 0;
	
	while (str[0] != 0) {
		 if (str[0] == '.'){
			 ndec = 1;
		} else {
			if ((str[0] > '9') || (str[0] < '0'))
				return d;
			d *= 10;
			d += str[0] - '0';
			if (ndec > 0)
				ndec++;
			if (ndec > numdec)	 // we reach the number of decimals...
				return d;
		}
		str++;
	}
	return d;
}
