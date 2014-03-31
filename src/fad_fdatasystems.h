// fad_fdatasystems.h: Class definition for the FlightDataSystems Fuel/Air-data unit
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

#ifndef FAD_FDATASYSTEMS_H
#define FAD_FDATASYSTEMS_H


#include "serial.h"


class fad_fdatasystems {
    // Members
    public:
        bool            good;           // A known good reading has been taken
    protected:
        serial *serialPtr;
        //shadinZ *shadinZPtr;
    // Funcs
    public:
        /**
         * Sample
         * DESCRIPTION:     Sample FlightDataSystems Fuel/air-data cooked data.
         * PRE-CONDITIONS:  Connected to a valid FlightDatSystems device (powered up and connected).
         * POST-CONDITIONS: good flag is FALSE, or all public angle data is correct.
         * EXCEPTIONS THROWN:  ??? TODO:
         * EXCEPTIONS HANDLED: None
         */
        bool    // Returns TRUE if new data was available
                        // or FALSE if the data remains unchanged as a result
                        // of calling this function.
            Sample (void);

        fad_fdatasystems(const char *port);

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
	void parseDataFrame(unsigned char *framePtr);
};

#endif
