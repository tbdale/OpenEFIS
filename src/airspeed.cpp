// airspeed.cpp: Class definition for the airspeed indicator
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

#include <math.h>
#include <stdio.h>

#include "exceptions.h"

#include "airspeed.h"

airspeed::airspeed (void)
{
    static float        das_filt [20];
    unsigned            i;
    float               tap;

    tap = .5;
    for (i = 0; i < NELEMENTS(das_filt)-1; i++)
    {
        das_filt [i] = tap;
        tap /= 2;
    }
    das_filt [NELEMENTS(das_filt)-1] = das_filt [NELEMENTS(das_filt)-2]; 
    hw = NULL;
    diff = new differentiate (100, das_filt, NELEMENTS(das_filt));
}

/**
 * Update
 * DESCRIPTION:     Read latest input from I/O board and update the airspeed read.
 * PRE-CONDITIONS:  
 * POST-CONDITIONS: The airspeed is updated or the "good" flag is FALSE.
 * EXCEPTIONS THROWN:  NO_IO_BOARD, BAD_IO_DRIVER
 * EXCEPTIONS HANDLED: 
 */
void airspeed::Update (void)
{

    if (hw == NULL)
        ThrowException (NO_IO_BOARD);
    if (hw->Sample (as))
    {
        float   das;
        diff->AddSample ((int)as);
        das = diff->Differentiate();
        as_prime = das * sample_rate;
        good = TRUE;
        // TODO: Add FDR function call here
        //printf ("Airspeed = %5u, das = %10f, sample_rate = %8f, as_prime = %8f\n",
        //        as, das, sample_rate, as_prime);
    }
}

/**
 * ReadCASTable
 * DESCRIPTION:     Read in the CAS table from a file.
 * PRE-CONDITIONS:  The given path exists in the form of a table of unsigned
 *                  integers with whitespace seperaters.
 * POST-CONDITIONS: cas table populated
 * EXCEPTIONS THROWN:  NO_SUCH_FILE
 * EXCEPTIONS HANDLED: None
 */
SyntaxError *       // Syntax error description, or NULL if file read in OK.
airspeed::ReadCASTable
(
 const char    *path    // Path to CAS table.
)
{
    // Null for now. No Calibration currently implemented.
    return NULL;
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
airspeed::DutyCycle
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
        ret = AIRSPEED_IDEAL_SAMPLE_PERIOD / main_loop_interval;
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

airspeed_hardware::airspeed_hardware (void)
{
    as_scale = 1.0;
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
airspeed_hardware::Sample
(
 unsigned  &value   // Returns the indicated airspeed in knots
)
{
    ThrowException (NO_IO_BOARD);
}

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
float   // See Description
airspeed_hardware::TimeBase (void) const
{
    ThrowException (NO_IO_BOARD);
}

airspeed_hardware::~airspeed_hardware () {}

