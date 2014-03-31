// ahrs_xbow.h: Class definition for the attitude heading reference system 
//  implementation by the XBow 500 series gyro.
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

// XBOW_FRAME_PERIOD: microseconds between the samples sent out the com port
// (right now is set to 100 samples/sec from factory but can be raised/lowered)
#define XBOW_FRAME_PERIOD  10000

// This class interfaces with the Xbow 500 series device that provides
// "cooked" e-gyro and accelerometer data.
class ahrs_xbow : public ahrs_hardware
{
    public:
        // public members
    protected:
	serial *serialPtr;

    public:
        /**
         * Sample
         * DESCRIPTION:     Sample xbow cooked data.
         * PRE-CONDITIONS:  Connected to a valid xbow device (powered up and connected).
         * POST-CONDITIONS: good flag is FALSE, or all public angle data is correct.
         * EXCEPTIONS THROWN:  NO_IO_BOARD, BAD_IO_DRIVER
         * EXCEPTIONS HANDLED: None
         */
        virtual bool    // Returns TRUE if new data was available
                        // or FALSE if the data remains unchanged as a result
                        // of calling this function.
            Sample (void);

        ahrs_xbow(void);
        ahrs_xbow(const char *port);


    protected:
        //float           thrust_vec;
        //const float     ang_scale;

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
	bool parseDataFrame(unsigned char*);
};

