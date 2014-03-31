// nav.h: Class definition for the NAV radio output reader
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

#define NAV_IDEAL_SAMPLE_PERIOD    1000000

class nav_hardware;

class nav {
    public:
        unsigned        obs;            // In degrees 1-360
        bool            cdi_good;       // A known good reading has been taken
        bool            gsi_good;       // A known good reading has been taken
        int             cdi;            // Course deviation indicator in 1/100 degrees deflection
        int             gsi;            // Glide slope indicator in 1/100 degrees deflection
        bool            to;             // True going to nav, false from nav
        float           cdi_prime, gsi_prime;  // Change in indicators

        /**
         * Update
         * DESCRIPTION:     Read latest input from I/O board and update the inicators
         * PRE-CONDITIONS:  
         * POST-CONDITIONS: Both glide slope and CDI are updated or their respective
         *                  "good" flags are false.
         * EXCEPTIONS THROWN:  NO_IO_BOARD, BAD_IO_DRIVER
         * EXCEPTIONS HANDLED: 
         */
        void Update (void);

        /**
         * DutyCycle
         * DESCRIPTION:     Compute the Update call duty cycle from
         *                  the main loop frequency (given) and the
         *                  hardware sample frequency and the sampling
         *                  requirements of the sensor.
         * PRE-CONDITIONS:  Hardware connected.
         * POST-CONDITIONS: Call frequency determined.
         * EXCEPTIONS THROWN:  NO_IO_BOARD
         * EXCEPTIONS HANDLED: None
         */
        virtual unsigned  // The number of times the main loop shall execute for
                          // every single time update is called for this class
        DutyCycle
        (
         unsigned    main_loop_interval      // The period in uS of the main loop
        );

        nav (void) {cdi_good = gsi_good = FALSE;}
    protected:
        nav_hardware   *hw;
        differentiate  *cdi_diff;
        differentiate  *gsi_diff;
        float           sample_rate;
};

extern nav     *TheNAVNeedles;

class nav_hardware
{
    public:
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
            Sample
            (
             int       &cdi,    // radial reading in 1/100th degrees
             int       &gsi     // glide slope deflection in 1/100th degrees
            );

        /**
         * TimeBase
         * DESCRIPTION:     Returns how many times per second a new history entry
         *                  arrives in the raw reading history buffer.
         * PRE-CONDITIONS:  None
         * POST-CONDITIONS: None
         * EXCEPTIONS THROWN:  None
         * EXCEPTIONS HANDLED: None
         */
        virtual float   // See Description
            TimeBase (void) const;

        nav_hardware(void) {cdi_scale = gsi_scale = 100.0;}

    protected:
        int             io_board_fd;            // File descriptor to I/O board reading
        float           cdi_scale, gsi_scale;
};
