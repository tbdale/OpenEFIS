// compass.h: Class definition for the compass heading reader
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

#ifndef COMPASS_H
#define COMPASS_H

#include "differentiate.h"
#include "syntax_error.h"
#include "exceptions.h"

#define COMPASS_IDEAL_SAMPLE_PERIOD    500000

class compass_hardware;

class compass {
    public:
        float           heading;        // In degrees 1-360
        float           heading_prime;  // In degrees / s
        bool            good;           // A known good reading has been taken

        unsigned        duty_cycle;

        /**
         * Update
         * DESCRIPTION:     Read latest input from I/O board and update the heading read.
         * PRE-CONDITIONS:  
         * POST-CONDITIONS: The heading is updated or the "good" flag is FALSE.
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
             compass_hardware *chw
            )
            {hw = chw;}

        /**
         * ReadCalHeadings
         * DESCRIPTION:     Read in the calibrated heading table from a file.
         * PRE-CONDITIONS:  The given path exists in the form of a table of unsigned
         *                  integers with whitespace seperaters.
         * POST-CONDITIONS: cas table populated
         * EXCEPTIONS THROWN:  NO_SUCH_FILE
         * EXCEPTIONS HANDLED: None
         */
        SyntaxError *       // Syntax error description, or NULL if file read in OK.
        ReadCalHeadings
        (
         const char    *path    // Path to CAS table.
        );

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

        compass (void);

    protected:
        compass_hardware       *hw;
        differentiate          *diff;
        float                   sample_rate;
};

extern compass *TheCompass;

class compass_hardware
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
             float     &x,      // heading in degrees or
                                // relative string of field in the x direction
             int       &y,      // relative strength of field in y direction
             int       &z       // relative strength of field in z direction
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
            TimeBase (void) const
        {
            ThrowException (NO_IO_BOARD);
        }

        compass_hardware (void) : cps_scale (1.0) {}

    protected:
        int             io_board_fd;            // File descriptor to I/O board reading
        float           cps_scale;
};

#endif
