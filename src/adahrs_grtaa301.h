// adahrs_grtaa301.h: Class definition for the attitude heading reference system 
//  (and airdata) implementation by the Grand Rapids Technologies GRT-AA-0301 series device.
//  ( http://www.grtavionics.com )
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
#include "ahrs.h"
#include "serial.h"

// GRT_FRAME_PERIOD: microseconds between the samples sent out the com port
// (right now is set to 100 samples/sec from factory but can be raised/lowered)
#define GRT_FRAME_PERIOD  10000 // TODO: May need to tweak

// This class interfaces with the Grand Rapids Technologies GRT-AA-0301 series device that provides
// "cooked" AHRS and airdata.
class adahrs_grtaa301 // TODO fix inheritance : public ahrs_hardware
{
    public:
        // public members
    protected:
	serial *serialPtr;

    public:
        /**
         * Sample
         * DESCRIPTION:     Sample GRT ad-ahrs cooked data.
         * PRE-CONDITIONS:  Connected to a valid GRT ad-ahrs device (powered up and connected).
         * POST-CONDITIONS: good flag is FALSE, or all public angle data is correct.
         * EXCEPTIONS THROWN:  NO_IO_BOARD, BAD_IO_DRIVER
         * EXCEPTIONS HANDLED: None
         */
        virtual bool    // Returns TRUE if new data was available
                        // or FALSE if the data remains unchanged as a result
                        // of calling this function.
            Sample (void);

        adahrs_grtaa301(const char *port);


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
         * parseMessage
         * DESCRIPTION:    Extracts data from serial frame and sets members
         * PRE-CONDITIONS:  None
         * POST-CONDITIONS: None
         * EXCEPTIONS THROWN:  None
         * EXCEPTIONS HANDLED: None
         */
	bool parseMessage(unsigned char*);

	void handleHighRatePrimaryDataMsg(unsigned char *msg);
	void handleLowRatePrimaryDataMsg(unsigned char *msg);
	void handleUserCalibrationMsg(unsigned char *msg);
	void handleMaintenanceMsg(unsigned char *msg);
	bool checksumOk(unsigned char *checkMsg, int startIdx, int stopIdx, int checksumIdx);
};

