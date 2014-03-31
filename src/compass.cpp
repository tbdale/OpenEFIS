// compass.cpp: Class definition for the compass heading reader
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

#include <math.h>
#include <stdio.h>

#include "exceptions.h"

#include "compass.h"

/**
 * Update
 * DESCRIPTION:     Read latest input from I/O board and update the heading read.
 * PRE-CONDITIONS:  
 * POST-CONDITIONS: The heading is updated or the "good" flag is FALSE.
 * EXCEPTIONS THROWN:  NO_IO_BOARD, BAD_IO_DRIVER
 * EXCEPTIONS HANDLED: 
 */
void compass::Update (void)
{
    float       x;
    int         y, z;

    if (hw == NULL)
        ThrowException (NO_IO_BOARD);
    if (hw->Sample (x, y, z))
    {
        float   dhe;
        float   newheading = x;

        if (heading - newheading > 180)
            newheading += 360;
        else if (newheading - heading > 180)
            newheading -= 360;
        diff->AddSample ((int) roundf (newheading));
        while (newheading > 360)
            newheading -= 360;
        while (newheading < 1)
            newheading += 360;
        heading = newheading;
        dhe = diff->Differentiate();
        heading_prime = (dhe * sample_rate);      // degrees / s
        //printf ("heading = %d, heading' = %f\n", heading, heading_prime);
        good = TRUE;
    }
}

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
compass::ReadCalHeadings
(
 const char    *path    // Path to CAS table.
)
{
    return NULL;
}

compass::compass (void)
{
    static float        cps_filt [8];
    unsigned            i;
    float               tap;

    tap = .5;
    for (i = 0; i < NELEMENTS(cps_filt)-1; i++)
    {
        cps_filt [i] = tap;
        tap /= 2;
    }
    cps_filt [NELEMENTS(cps_filt)-1] = cps_filt [NELEMENTS(cps_filt)-2]; 
    hw = NULL;
    diff = new differentiate (100, cps_filt, NELEMENTS(cps_filt));
}

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
unsigned  // The number of times the main loop shall execute for
                  // every single time update is called for this class
compass::DutyCycle
(
 unsigned    main_loop_interval      // The period in uS of the main loop
)
{
    unsigned    ret;

    if (hw == NULL)
        ThrowException (NO_IO_BOARD);
    sample_rate = hw->TimeBase();
    if (sample_rate == 0)
    {
        ret = COMPASS_IDEAL_SAMPLE_PERIOD / main_loop_interval;
        if (ret == 0)
            ret = 1;
        sample_rate = 1000000.0 / ((float) ret * main_loop_interval);
    } else {
        // loops / call = 1 / (us/loop * call/s * 1s/1000000us) =
        // (1000000us / 1s) / (us/loop * call/s)
        ret = (unsigned) roundf (1000000.0 /
                                ((float) main_loop_interval * sample_rate));
    }
    return ret;
}

/**
 * Sample
 * DESCRIPTION:     Sample sensor data.
 * PRE-CONDITIONS:  io_board_fd is open to a valid device driver.
 * POST-CONDITIONS: good flag is FALSE, or all public course data is correct.
 * EXCEPTIONS THROWN:  NO_IO_BOARD, BAD_IO_DRIVER
 * EXCEPTIONS HANDLED: None
 */
bool    // Returns TRUE if new data was available
        // or FALSE if the data remains unchanged as a result
        // of calling this function.
compass_hardware::Sample
(
 float     &x,      // heading in degrees or
                    // relative string of field in the x direction
 int       &y,      // relative strength of field in y direction
 int       &z       // relative strength of field in z direction
)
{
    ThrowException (NO_IO_BOARD);
}

