// nav.cpp: Class definition for the NAV radio output reader
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

#include "exceptions.h"

#include "nav.h"

/**
 * Update
 * DESCRIPTION:     Read latest input from I/O board and update the inicators
 * PRE-CONDITIONS:  
 * POST-CONDITIONS: Both glide slope and CDI are updated or their respective
 *                  "good" flags are false.
 * EXCEPTIONS THROWN:  NO_IO_BOARD, BAD_IO_DRIVER
 * EXCEPTIONS HANDLED: 
 */
void nav::Update (void)
{
    int         rawcdi, rawgsi;

    if (hw == NULL)
        ThrowException (NO_IO_BOARD);
    if (hw->Sample (rawcdi, rawgsi))
    {
        // Normalize heading
        while (rawcdi > 36000)
            rawcdi -= 36000;
        while (rawcdi <= 0)
            rawcdi += 36000;
        cdi = ((obs * 100) - rawcdi);
        to = FALSE;
        if (cdi < -180)
            cdi += 360;
        if (cdi > 180)
            cdi -= 360;
        if (cdi < -90)
        {
            to = TRUE;
            cdi += 180;
        } else if (cdi > 90)
        {
            to = TRUE;
            cdi -= 180;
        }

        gsi = rawgsi;
        cdi_diff->AddSample (cdi);
        gsi_diff->AddSample (rawgsi);
        float c = cdi_diff->Differentiate();
        float g = gsi_diff->Differentiate();
        cdi_prime = c * sample_rate;
        gsi_prime = g * sample_rate;
        cdi_good = TRUE;
        gsi_good = TRUE;
    }
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
nav::DutyCycle
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
        ret = NAV_IDEAL_SAMPLE_PERIOD / main_loop_interval;
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
nav_hardware::Sample
(
 int       &cdi,    // radial reading in 1/100th degrees
 int       &gsi     // glide slope deflection in 1/100th degrees
)
{
    ThrowException (NO_IO_BOARD);
}

/**
 * TimeBase
 * DESCRIPTION:     Returns how many times per second a new history entry
 *                  arrives in the raw reading history buffer.
 * PRE-CONDITIONS:  None
 * POST-CONDITIONS: None
 * EXCEPTIONS THROWN:  None
 * EXCEPTIONS HANDLED: None
 */
float   // See Description
nav_hardware::TimeBase (void) const
{
    ThrowException (NO_IO_BOARD);
}

