// gps.cpp: Class definition for the GPS data reader
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

#include "gps.h"

/**
 * Update
 * DESCRIPTION:     Read latest input from I/O board and update the GPS data.
 * PRE-CONDITIONS:  
 * POST-CONDITIONS: The data is updated or the "good" flag is FALSE.
 * EXCEPTIONS THROWN:  NO_IO_BOARD, BAD_IO_DRIVER
 * EXCEPTIONS HANDLED: 
 */
void gps::Update (void)
{
    if (hw == NULL)
        ThrowException (NO_IO_BOARD);
    //if (hw->Sample (lat, lng, ground_track, ground_speed, unix_time))
    if (hw->Sample())
    {
        dv->AddSample (ground_speed);
        float x = dv->Differentiate();
        delta_v = x * sample_rate;
        good = TRUE;
    }
}

static float    deltav_filt [] =
{ 0.6, 0.3, 0.07, 0.03};

gps::gps (void)
{
    hw = NULL;
    dv = new differentiate (NELEMENTS(deltav_filt), deltav_filt, NELEMENTS(deltav_filt));
    good = FALSE;
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
gps::DutyCycle
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
        ret = GPS_IDEAL_SAMPLE_PERIOD / main_loop_interval;
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

gps_hardware::gps_hardware(void)
{
    good = FALSE;
    lat = 0;
    lng = 0;
    track = 0;
    speed = 0;
    tm = 0;
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
gps_hardware::Sample
(
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
gps_hardware::TimeBase (void) const
{
    ThrowException (NO_IO_BOARD);
}

