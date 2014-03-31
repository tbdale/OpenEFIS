// ahrs_cooked.cpp: Class definition for the attitude heading reference system
//                for sensors which provide absolute data rather than just
//                rate data.
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

#include "ahrs.h"
#include "ahrs_cooked.h"

ahrs_cooked::ahrs_cooked ()
 { ahrs::ahrs(); }

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
ahrs_cooked::compute_pitch
(
 unsigned       dt,     // Delta time to use when integrating data. Set to zero
                        // when taxiing to not use angular rate sensor data.
                        // Units in microseconds.
 float          dv      // Delta velocity in knots to filter pitch with.
                        // Comes from GPS when flying or the wheel speed when taxiing.
)
{
    pitch_angle = data_source->pitch_cooked;
    pitch_angle_prime = data_source->ang_pitch;
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
ahrs_cooked::compute_roll_flying
(
 unsigned       dt      // Delta time to use when integrating data.
                        // Units in microseconds.
)
{
    roll_angle = data_source->roll_cooked;
    roll_angle_prime = data_source->ang_roll;
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
ahrs_cooked::compute_roll_still
(void)
{
    roll_angle = data_source->roll_cooked;
    roll_angle_prime = data_source->ang_roll;
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
ahrs_cooked::compute_heading_flying
(
 unsigned       dt      // Delta time to use when integrating data.
                        // Units in microseconds.
)
{
    heading_angle = data_source->heading_cooked;
    heading_angle_prime = data_source->ang_head;
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
ahrs_cooked::compute_heading_still
(void)
{
    heading_angle = data_source->heading_cooked;
    heading_angle_prime = data_source->ang_head;
}

/**
 * compute_yaw
 * DESCRIPTION:     Compute yaw (slip/skid) based on accelerometer data
 * PRE-CONDITIONS:  The aircraft is stationary on the ground
 * PRE-CONDITIONS:  raw sensor data current.
 * POST-CONDITIONS: yaw_angle is accurate
 * EXCEPTIONS THROWN:  
 * EXCEPTIONS HANDLED: 
 */
void
ahrs_cooked::compute_yaw
(void)
{
}
