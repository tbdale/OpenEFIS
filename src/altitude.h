// altitude.h: Class definition for the altitude encoder
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

#include "differentiate.h"

#define ALTITUDE_IDEAL_SAMPLE_PERIOD    750000

class altitude_hardware;

class altitude {
    public:
        int             alt;            // Indicated altitude of aircraft
        int             pressure_alt;   // Pressure altitude of aircraft
        float           alt_prime;      // Change in altitude in fpm (VSI)
        float           altimeter;      // Altimeter setting in "Hg
        bool            good;           // A known good reading has been taken

        unsigned        duty_cycle;

        /**
         * Update
         * DESCRIPTION:     Read latest input from I/O board and update the altitude read.
         * PRE-CONDITIONS:  
         * POST-CONDITIONS: The altitude is updated or the "good" flag is FALSE.
         * EXCEPTIONS THROWN:  NO_IO_BOARD, BAD_IO_DRIVER
         * EXCEPTIONS HANDLED: 
         */
        void Update (void);

        /**
         * ConnectHardware
         * DESCRIPTION:     Connect the behavior hiding object (this) to the hardware
         *                  hiding class.
         * PRE-CONDITIONS:  
         * POST-CONDITIONS: 'hw' assigned to real hardware object
         * EXCEPTIONS THROWN:  
         * EXCEPTIONS HANDLED: 
         */
        void ConnectHardware
            (
             altitude_hardware      *alhw
            )
            {hw = alhw;}

        /**
         * SetAltimeter
         * DESCRIPTION:     Set the altimeter setting to convert from pressure altitude
         *                  to actual altitude.
         * PRE-CONDITIONS:  None
         * POST-CONDITIONS: Barometric pressure known by system
         * EXCEPTIONS THROWN:  None
         * EXCEPTIONS HANDLED: None
         */
        void SetAltimeter
        (
         float          alt_setting     // In "Hg
        );

        altitude::altitude (void);

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

    protected:
        altitude_hardware      *hw;
        differentiate          *diff;
        float                   sample_rate;
};

extern altitude        *TheAltitude;

class altitude_hardware
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
             int       &value   // Returns the pressure altitude in feet MSL
            );

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

        altitude_hardware (void);

    protected:
        int             io_board_fd;            // File descriptor to I/O board reading
        float           alt_scale;
};
