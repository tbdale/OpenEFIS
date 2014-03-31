// altitude.cpp: Class definition for the altitude encoder
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

#include "altitude.h"

altitude::altitude (void)
{
    static float        dal_filt [20];
    unsigned            i;
    float               tap;

    tap = .5;
    for (i = 0; i < NELEMENTS(dal_filt)-1; i++)
    {
        dal_filt [i] = tap;
        tap /= 2;
    }
    dal_filt [NELEMENTS(dal_filt)-1] = dal_filt [NELEMENTS(dal_filt)-2]; 
    hw = NULL;
    diff = new differentiate (100, dal_filt, NELEMENTS(dal_filt));
}

/**
 * Update
 * DESCRIPTION:     Read latest input from I/O board and update the altitude read.
 * PRE-CONDITIONS:  
 * POST-CONDITIONS: The altitude is updated or the "good" flag is FALSE.
 * EXCEPTIONS THROWN:  NO_IO_BOARD, BAD_IO_DRIVER
 * EXCEPTIONS HANDLED: 
 */
void altitude::Update (void)
{
    if (hw == NULL)
        ThrowException (NO_IO_BOARD);
    if (hw->Sample (alt))
    {
        float   dal;
        diff->AddSample (alt);
        dal = diff->Differentiate();
        alt_prime = (dal * sample_rate) * 60 * 1000 / (ALTITUDE_IDEAL_SAMPLE_PERIOD / 1000);      // fpm
        good = TRUE;
        // TODO: Add FDR function call here
        //printf ("Altitude = %5d, dal = %10f, sample_rate = %8f, alt_prime = %8f\n",
        //        alt, dal, sample_rate, alt_prime);
    }
}

/**
 * SetAltimeter
 * DESCRIPTION:     Set the altimeter setting to convert from pressure altitude
 *                  to actual altitude.
 * PRE-CONDITIONS:  None
 * POST-CONDITIONS: Barometric pressure known by system
 * EXCEPTIONS THROWN:  None
 * EXCEPTIONS HANDLED: None
 */
void altitude::SetAltimeter
(
 float          alt_setting     // In "Hg
)
{
    altimeter = alt_setting;
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
altitude::DutyCycle
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
        ret = ALTITUDE_IDEAL_SAMPLE_PERIOD / main_loop_interval;
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
altitude_hardware::Sample
(
 int       &value   // Returns the pressure altitude in feet MSL
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
altitude_hardware::TimeBase (void) const
{
    ThrowException (NO_IO_BOARD);
}

altitude_hardware::altitude_hardware (void) : alt_scale (1.0) {}

