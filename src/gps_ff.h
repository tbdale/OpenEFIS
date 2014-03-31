// gps_ff.h: Class definition for a gps that conforms to the 
//  TSO C145 style GPS/WAAS sensors.
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

#include "serial.h"
#include "gps.h"


#define FRAME_BUF_SIZE 20000 // TODO: move this buffer stuff in the serial class???
// TODO: ...because I have this declared in two header files so far - could cause confusion

// This class interfaces with the gps/waas tso c145 confroming device that provides
// "cooked" gps data with fault detection, exclusion, and raim.
class gps_ff : public gps_hardware
{
    public:
        //int good; // TODO: Take out when inherit from parent

    protected:
	unsigned char frameDataBuf[FRAME_BUF_SIZE]; // TODO: sized correctly?
        serial *serialPtr;
        double gps_altitude;

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


        gps_ff(void);
        gps_ff(const char *port);


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
         * TimeBase
         * DESCRIPTION:     Returns how many times per second a new history entry
         *                  arrives. Returns 0 if a sample is taken every time 'Sample'
         *                  is called.
         * PRE-CONDITIONS:  None
         * POST-CONDITIONS: None
         * EXCEPTIONS THROWN:  None
         * EXCEPTIONS HANDLED: None
         */
        virtual float   // See Description
            TimeBase (void) const;

        /**
         * parseNavPacket
         * DESCRIPTION:    Extracts data from serial frame and sets members
         * PRE-CONDITIONS:  None
         * POST-CONDITIONS: None
         * EXCEPTIONS THROWN:  None
         * EXCEPTIONS HANDLED: None
         */
	bool parseNavPacket(unsigned char*);
	bool parsePfdeResponsePacket(unsigned char *);
	bool parseGwssStatusPacket(unsigned char *);
	double parseDoubleFromPacket(unsigned char * pktPtr, int startIdx);
	float parseFloatFromPacket(unsigned char *, int startIdx);
};

