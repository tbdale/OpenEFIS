// xpdr_sl70r.h: Class definition for the Apollo remote transponder (SL70R)
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//

#include "serial.h"
#include "constants.h"


#define XPDR_SAMPLE_PERIOD  1000000 // Once a sec

class xpdr_sl70r {
    // Members
    public:
        bool            good;           // A known good reading has been taken
        int squawkAltFlLvl;
        bool errorAltOutOfRange;
        bool errorHardwareBad;
	char modeCharReceive; // "O"=standby, "A"=ModeA; "C"=ModeC
	char identCharReceive; // "-"=inactive; "I"=enabled
	int  squawkCodeReceive;
	int  replyCount;
	char softwareVersion[80];
    protected:
        serial *serialPtr;
        int altFt;
        int degC;
	char identCharSend; // "-"=inactive; "I"=enabled
	char modeCharSend; // "O"=standby, "A"=ModeA; "C"=ModeC
	int  squawkCodeSend;
	unsigned short status2Bytes;
    // Funcs
    public:
        /**
         * Sample
         * DESCRIPTION:     Sample sensor data.
         * PRE-CONDITIONS:  Comm port is open to a valid device driver.
         * POST-CONDITIONS: good flag is FALSE, or all public course data is correct.
         * EXCEPTIONS THROWN:  NO_IO_BOARD, BAD_IO_DRIVER TODO:???
         * EXCEPTIONS HANDLED: None
         */
        bool    // Returns TRUE if new data was available
	  // or FALSE if the data remains unchanged as a result
	  // of calling this function.
        Sample(void);

        xpdr_sl70r(const char *port);
	void sendMsgToUnit(char *msg, int numChars);
	unsigned short calculateChecksum(char *ptr, int startIdx, int stopIdx);
	bool  checksumOk(char *msg, int startIdx, int stopIdx, int checksumIdx);
	void resetUnit(void);
	
	void setAltitude(int altFeet, int degC);
	void setModeA(void);
	void setModeC(void);
	void setModeStandby(void);
	void enableIdent();
	void disableIdent();
	void setSquawk(int squawkInt);
	void setSquawk(char *squawkStr);
    protected:   
        /**
         * initialize
         * DESCRIPTION:    Conditions device/comm port for reading data
         * PRE-CONDITIONS:  None
         * POST-CONDITIONS: None
         * EXCEPTIONS THROWN:  None
         * EXCEPTIONS HANDLED: None
         */
	void initialize(void);

        /**
         * parseDataFrame
         * DESCRIPTION:    Extracts data from serial frame and sets members
         * PRE-CONDITIONS:  None
         * POST-CONDITIONS: None
         * EXCEPTIONS THROWN:  None
         * EXCEPTIONS HANDLED: None
         */
	//void parseDataFrame(unsigned char *framePtr);
	void setMode(void);

	void parseMessage(char *);

	void handleAltMsg(char *msg);

	void handleStatusMsg(char *msg);

	void handleModeMsg(char *msg);

	void handleReplyCountMsg(char *msg);

	void handleSoftwareVersionMsg(char *msg);

	void handleAltHoldMsg(char *msg);
};
