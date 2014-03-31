// ahrs.cpp: Member functions of AHRS class (attitude heading reference system)
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
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "utilities.h"
#include "exceptions.h"

#include "ahrs.h"
#include "gps.h"
#include "compass.h"

/**
 * SampleAndCompute
 * DESCRIPTION:     Sample raw sensor data and compute filtered data
 * PRE-CONDITIONS:  The aircraft is flying, not still.
 *                  Call SampleAndComputeStill when aircraft is on the ground.
 *                  io_board_fd is open to a valid device driver.
 * POST-CONDITIONS: good flag is FALSE, or all public angle data is correct.
 * EXCEPTIONS THROWN:  NO_IO_BOARD, BAD_IO_DRIVER
 * EXCEPTIONS HANDLED: None
 */
void ahrs::SampleAndCompute (void)
{

    if (data_source == NULL)
        ThrowException (NO_IO_BOARD);
    if (data_source->Sample() == FALSE)
        return;         // No new sensor data available, so don't compute anything

    /******  Compute AHRS from raw sensor data  *******/
    if (TheGPS->good) {
        compute_pitch(data_source->dt, TheGPS->delta_v);
        good = TRUE;
    } else {
        compute_pitch(data_source->dt, 0);
        good = FALSE;
    }
    compute_roll_flying (data_source->dt);
    compute_heading_flying (data_source->dt);
    compute_yaw();

//    static int i = 0;
//    if ((++i % 100) == 0)
//    {
//        printf ("roll = %10f, pitch = %10f, heading = %10f\n",
//                roll_angle * 180 / M_PI,
//                pitch_angle * 180 / M_PI,
//                heading_angle * 180 / M_PI);
//    }
}

/**
 * SampleAndComputeStill
 * DESCRIPTION:     Sample raw sensor data and compute filtered data assuming the aircraft
 *                  is on the ground.
 * PRE-CONDITIONS:  The aircraft is still, not flying.
 *                  Call SampleAndCompute when aircraft is flying.
 *                  io_board_fd is open to a valid device driver.
 * POST-CONDITIONS: good flag is FALSE, or all public angle data is correct.
 * EXCEPTIONS THROWN:  NO_IO_BOARD, BAD_IO_DRIVER
 * EXCEPTIONS HANDLED: None
 */
void ahrs::SampleAndComputeStill (void)
{

    if (data_source == NULL)
        ThrowException (NO_IO_BOARD);
    if (data_source->Sample() == FALSE)
        return;         // No new sensor data available, so don't compute anything

    /******  Compute AHRS from raw sensor data  *******/
    compute_pitch(0, 0);
    compute_roll_still ();
    compute_heading_still ();
    compute_yaw();
}

/**
 * compute_pitch
 * DESCRIPTION:     Compute the pitch by integrating angular rate sensor data and
 *                  filtering it with data from the accelerometers
 * PRE-CONDITIONS:  raw sensor data current.
 * POST-CONDITIONS: pitch_angle updated
 * EXCEPTIONS THROWN:  
 * EXCEPTIONS HANDLED: 
 */
void
ahrs::compute_pitch
(
 unsigned       dt,     // Delta time to use when integrating data. Set to zero
                        // when still to not use angular rate sensor data.
                        // Units in microseconds.
 float          dv      // Delta velocity in knots/s to filter pitch with.
                        // Comes from GPS when flying or the wheel speed when still.
)
{
    float       estimated_pitch, integrated_pitch;
    static int  i = 0;

    estimated_pitch = atan ((data_source->accel_thrust -
                                (dv / KNOTS_PER_METER_S)) /
                            data_source->accel_lift);
    //printf ("dv = %f, thrust = %f, lift = %f, es = %f\n",
    //        dv / KNOTS_PER_METER_S,
    //        data_source->accel_thrust,
    //        data_source->accel_lift,
    //        estimated_pitch * 180.0 / M_PI);

    if (dt != 0)
    {
        // dt > 0 when we are flying and regularly sampling the sensors

        // ang_rate_constant = (radians / s) / unit sampled from angular rate sensors
        pitch_angle_prime = data_source->ang_pitch;
        integrated_pitch = pitch_angle + pitch_angle_prime * ((float )dt / 1000000.0);

        // Slave pitch angle to estimated pitch
        // noise_constant = some value greater than drift introduced by random noise.
        // noise_constant = radians / s max drift towards estimated value
        pitch_angle = integrated_pitch +
                   ((estimated_pitch - integrated_pitch) *
                    noise_constant * ((float ) dt / 1000000.0));
        if ((i++ % 40) == 0)
            printf ("estimated pitch = %f, int pitch = %f, pitch = %f\n",
                    estimated_pitch * 180.0 / M_PI,
                    integrated_pitch * 180.0 / M_PI,
                    pitch_angle * 180.0 / M_PI);
    } else {
        // dt = 0 when we are flying and regularly sampling the sensors
        // Since we are on the taxi way the pitch and roll will be equal to the vectors
        // sensed by the accelerometers.
        pitch_angle = estimated_pitch;
        pitch_angle_prime = 0;
    }
}


/**
 * compute_roll_flying
 * DESCRIPTION:     Compute the roll angle
 * PRE-CONDITIONS:  The aircraft is flying
 * PRE-CONDITIONS:  raw sensor data current.
 * POST-CONDITIONS: roll_angle is accurate
 * EXCEPTIONS THROWN:  
 * EXCEPTIONS HANDLED: 
 */
void
ahrs::compute_roll_flying
(
 unsigned       dt      // Delta time to use when integrating data.
                        // Units in microseconds.
)
{
    float       estimated_roll, integrated_roll;
    static int  i = 0;

    roll_angle_prime = data_source->ang_roll;
    integrated_roll = roll_angle + (roll_angle_prime * (float )dt) / 1000000.0;
    if (TheGPS->good)
    {
        estimated_roll = atan (TheGPS->ground_speed / KNOTS_PER_METER_S
                                * data_source->ang_head / LOCAL_GRAVITY);
        // yaw_roll_constant: The amount uncoordinated flight can contribute to heading
        // changes to provide the appearance of or the absense of roll.
        // yaw_roll_constant = radians / unit sampled from yaw accelerometer
        estimated_roll -= yaw_roll_constant * data_source->accel_yaw;
    } else {
        good = FALSE;
        estimated_roll = integrated_roll;
    }
    // Slave roll angle to estimated roll
    roll_angle = integrated_roll +
               (((estimated_roll - integrated_roll) *
                noise_constant * (float ) dt) / 1000000.0);
    if ((i++ % 40) == 0)
    {
//        printf ("estimated roll = %f, int roll = %f, roll = %f\n",
//                estimated_roll * 180.0 / M_PI,
//                integrated_roll * 180.0 / M_PI,
//                roll_angle * 180.0 / M_PI);
    }
}

/**
 * compute_roll_still
 * DESCRIPTION:     Compute the roll angle based entirely on the g angle from accelerometer data
 * PRE-CONDITIONS:  The aircraft is stationary on the ground
 * PRE-CONDITIONS:  raw sensor data current.
 * POST-CONDITIONS: roll_angle is accurate
 * EXCEPTIONS THROWN:  
 * EXCEPTIONS HANDLED: 
 */
void
ahrs::compute_roll_still
(void)
{
    // Since we are on the taxi way the pitch and roll will be equal to the vectors
    // sensed by the accelerometers.
    roll_angle = atan2 (data_source->accel_yaw, data_source->accel_lift);
    roll_angle_prime = 0;
}

/**
 * compute_heading_flying
 * DESCRIPTION:     Compute the heading angle of aircraft as slaved to the compass
 * PRE-CONDITIONS:  The aircraft is flying
 * PRE-CONDITIONS:  raw sensor data current.
 * POST-CONDITIONS: heading_angle is accurate
 * EXCEPTIONS THROWN:  
 * EXCEPTIONS HANDLED: 
 */
void
ahrs::compute_heading_flying
(
 unsigned       dt      // Delta time to use when integrating data.
                        // Units in microseconds.
)
{
    float       estimated_heading, integrated_heading;

    heading_angle_prime = data_source->ang_head;
    integrated_heading = heading_angle + heading_angle_prime * ((float )dt / 1000000.0);

    // Get heading reading from compass
    if (TheCompass->good)
    {
        estimated_heading = TheCompass->heading / DEGREES_PER_RADIAN;
    } else {
        good = FALSE;
        estimated_heading = integrated_heading;
    }
    // Slave heading angle to compass heading
    heading_angle = integrated_heading +
               ((estimated_heading - integrated_heading) *
                noise_constant * ((float ) dt / 1000000.0));
}

/**
 * compute_heading_still
 * DESCRIPTION:     Compute the heading angle of aircraft as slaved to the compass
 * PRE-CONDITIONS:  The aircraft is stationary on the ground
 * PRE-CONDITIONS:  raw sensor data current.
 * POST-CONDITIONS: heading_angle is accurate
 * EXCEPTIONS THROWN:  
 * EXCEPTIONS HANDLED: 
 */
void
ahrs::compute_heading_still
(void)
{
    heading_angle_prime = 0;
    if (TheCompass->good)
        heading_angle = TheCompass->heading / DEGREES_PER_RADIAN;
    else
        good = FALSE;
}

/**
 * compute_yaw
 * DESCRIPTION:     Compute yaw (slip/skid) based on accelerometer data
 * PRE-CONDITIONS:  
 * PRE-CONDITIONS:  raw sensor data current.
 * POST-CONDITIONS: yaw_angle is accurate
 * EXCEPTIONS THROWN:  
 * EXCEPTIONS HANDLED: 
 */
void
ahrs::compute_yaw
(void)
{
    yaw_angle = atan2 (data_source->accel_yaw, LOCAL_GRAVITY);
    //printf ("yaw = %f\n", yaw_angle * 180 / M_PI);
}

/**
 * InitConstants
 * DESCRIPTION:     Read sensor constants from a file.
 * PRE-CONDITIONS:  The given path exists and is in the correct format.
 * POST-CONDITIONS: sensor constants initialized
 * EXCEPTIONS THROWN:  NO_SUCH_FILE
 * EXCEPTIONS HANDLED: None
 */
SyntaxError *
ahrs::InitConstants (const char *path)
{
    SyntaxError        *ret = NULL;
    FILE               *cfile;
    unsigned            line_num;
    char                line_text [256];
    char                constant_name [256];
    float               value;

    cfile = fopen (path, "r");
    if (cfile == NULL)
        ThrowException (NO_SUCH_FILE);

    line_num = 1;
    while (fgets (line_text, sizeof (line_text), cfile))
    {
        sscanf (line_text, "%s %f", constant_name, &value);
        if (strcmp (constant_name, "noise_constant") == 0) { 
            noise_constant = value;
        } else if (strcmp (constant_name, "yaw_roll_constant") == 0) { 
            yaw_roll_constant = value;
        } else {
            ret = new SyntaxError (line_num, 0, "Unknown constant");
            break;
        }
    }
    fclose (cfile);
    return (ret);
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
unsigned
ahrs::DutyCycle
(
 unsigned    main_loop_interval      // The period in uS of the main loop
)
{
    return 1;
}

ahrs::ahrs (void)
{
    good = FALSE;

    roll_angle = 0;
    pitch_angle = 0;
    heading_angle = 0;
    yaw_angle = 0;

    roll_angle_prime = 0;
    pitch_angle_prime = 0;
    heading_angle_prime = 0;
    yaw_angle_prime = 0;

    noise_constant = 0;
    yaw_roll_constant = 0;
    
    data_source = NULL;
}

/**
 * Sample
 * DESCRIPTION:     Sample sensor data.
 * PRE-CONDITIONS:  io_board_fd is open to a valid device driver.
 * POST-CONDITIONS: good flag is FALSE, or all public angle data is correct.
 * EXCEPTIONS THROWN:  NO_IO_BOARD, BAD_IO_DRIVER
 * EXCEPTIONS HANDLED: None
 */
bool    // Returns TRUE if new data was available
        // or FALSE if the data remains unchanged as a result
        // of calling this function.
ahrs_hardware::Sample (void)
{
    ThrowException (NO_IO_BOARD);
}
ahrs_hardware::ahrs_hardware (void)
{ang_scale = M_PI / 180.0; accel_scale = 1.0;}
