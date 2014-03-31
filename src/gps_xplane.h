// gps_xplane.h: Class definition for the GPS data reader for X-Plane simulation
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

#include "differentiate.h"
#include "gps.h"

class gps_xplane : public gps_hardware
{
    public:
        bool            good;
        float           altitude;       // for altitude_xplane to read
        /**
         * Sample
         * DESCRIPTION:     Sample sensor data.
         * PRE-CONDITIONS:  io_board_fd is open to a valid device driver.
         * POST-CONDITIONS: good flag is FALSE, or all public course data is correct.
         * EXCEPTIONS THROWN:  NO_IO_BOARD, BAD_IO_DRIVER
         * EXCEPTIONS HANDLED: None
         */
        virtual bool    // Returns TRUE if new data was available
                        // or FALSE if the data remains unchanged as a result
                        // of calling this function.
            Sample ();

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
         * ConnectAirspeed
         * DESCRIPTION:     Connect this object to the airspeed object which has the ground speed
         * PRE-CONDITIONS:  None
         * POST-CONDITIONS: None
         * EXCEPTIONS THROWN:  None
         * EXCEPTIONS HANDLED: None
         */
        void ConnectAirspeed
            (
             airspeed_xplane *a
            )
            {as = a;}

        gps_xplane (void);
    protected:
        unsigned        oversample_counter;
        airspeed_xplane*as;
};
