// comm_sl40.h: Class definition for the Apollo comm transceiver interface (SL40))
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


#define COMM_SAMPLE_PERIOD  1000000 // Once a sec

class comm_sl40 {
    // Members
    public:
        bool good;           // A known good reading has been taken
	char softwareVersion[8];
    protected:
        serial *serialPtr;
	int activeFrequencyKhz;
	int standbyFrequencyKhz;
	bool monitorStandbyFlag;
	char transceiverStatus;
	char squelchTestSetting;

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

	// Constructor
        comm_sl40(const char *port);

	void parseMessage(char *framePtr);

	void calculateChecksum(char *ptr, int startIdx, int stopIdx, char *checksum);

	bool  checksumOk(char *msg, int startIdx, int stopIdx, int checksumIdx);

	
        /** 
         * setStandbyFrequency
         * DESCRIPTION: Sets the standby frequency and monitor/no_monitor flag
         */
	void setActiveFrequencyKhz(int kHz);

        /** 
         * getStandbyFrequency
         * DESCRIPTION: Gets the standby frequency
         * RETURNS: int khZ
	 * NOTE: Sample() needs to be called on routine basis for frequency polling
         *       of the comm unit.
         */
	int getActiveFrequencyKhz();

        /** 
         * setStandbyFrequency
         * DESCRIPTION: Sets the standby frequency and monitor/no_monitor flag
         */
	void setStandbyFrequencyKhz(int kHz);

        /** 
         * getStandbyFrequency
         * DESCRIPTION: Gets the standby frequency
         * RETURNS: int khZ
	 * NOTE: Sample() needs to be called on routine basis for frequency polling
         *       of the comm unit.
         */
	int getStandbyFrequencyKhz();

        /** 
         * setMonitorStandby
         * DESCRIPTION: Sets the transceiver to monitor the standby frequency
         */
	void setMonitorStandby(bool monitorStandbyFlag);


        /** 
         * getTransceiverStatus
         * DESCRIPTION: Gets the standby frequency
         * RETURNS: one char-> R=normal receive, M=monitor selected, 
	 *                     T=transmit enable, S=stuck mic
         */
	char getTransceiverStatus();

        /** 
         * getSquelchStatus
         * DESCRIPTION: Returns the squelch status
         * RETURNS: 0=automatic 1=test selected
         */
	char getSquelchStatus();

        /** 
         * setNewFrequencyListTag
         * DESCRIPTION: Used to input a new ident/tag for a freq list (twr,grd,apr,etc.),
         *              and resets the remote frequency input pointer.
         */
	void setNewFrequencyListTag(char *tag4Chars, char listNum);

        /** 
         * setNewFrequencyListItem
         * DESCRIPTION: Used to input a new freq to a freq list (twr,grd,apr,etc.),
         */
	void setNewFrequencyListItem(char listNum, char freqType, int freqKhz);
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

};
