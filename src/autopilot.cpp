// autopilot.cpp: Member functions for the autopilot class
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

#include <sys/time.h>
#include <math.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "constants.h"
#include "utilities.h"

#include "ahrs.h"
#include "altitude.h"
#include "airspeed.h"
#include "nav.h"
#include "autopilot.h"

/**
 * Disengage
 * DESCRIPTION:     Disengage autopilot servos from controls
 * PRE-CONDITIONS:
 * POST-CONDITIONS: Servos inactive, autopilot stopped
 * EXCEPTIONS THROWN: NO_SERVOS
 * EXCEPTIONS HANDLED: 
 */
void autopilot::Disengage (void)
{
    roll_engaged = FALSE;
    pitch_engaged = FALSE;
    // Set servos to open circuit
    if (hw == NULL)
        ThrowException (NO_SERVOS);
    hw->servo_state_change (TRUE, FALSE, TRUE, FALSE, FALSE, FALSE);
}

/**
 * SetHeading
 * DESCRIPTION:     Set the desired heading for the autopilot
 * PRE-CONDITIONS:  The aircraft is flying and the AHRS has good data. If the new heading
 *                  is diametrically opposed to the present heading, the aircraft will
 *                  turn right. If a left turn is desired, set the heading to an interim
 *                  heading to the left in order to "lead" the autopilot in the desired
 *                  direction.
 * POST-CONDITIONS: Force and trim variables updated, autopilot is engaged.
 * EXCEPTIONS THROWN:  NO_AHRS, NOT_FLYING, NO_SERVOS
 * EXCEPTIONS HANDLED: 
 */
void
autopilot::SetHeading
(
 unsigned       heading         // New heading in degrees 1-360
)
{
    assert (heading <= 360);
    if (hw == NULL)
        ThrowException (NO_SERVOS);
    mode = AM_MANUAL;
    desired_heading = heading;
    if (roll_engaged == FALSE)
    {
        roll_engaged = TRUE;
        compute_initial_roll();
    }
    hw->servo_state_change (TRUE, TRUE, FALSE, FALSE, FALSE, FALSE);
}

/**
 * SetAltitude
 * DESCRIPTION:     Set the desired altitude for the autopilot
 * PRE-CONDITIONS:  The aircraft is flying and the AHRS has good data.
 * POST-CONDITIONS: Force and trim variables updated, autopilot is engaged.
 * EXCEPTIONS THROWN:  NO_ALTITUDE, NO_AIRSPEED, NOT_FLYING, NO_SERVOS
 * EXCEPTIONS HANDLED: 
 */
void
autopilot::SetAltitude
(
 unsigned       altitude,       // New altitude in feet MSL
 unsigned       climb_airspeed, // The target airspeed for a protracted climb
 unsigned       descent_airspeed  // The target airspeed for a protracted descent
)
{
    if (hw == NULL)
        ThrowException (NO_SERVOS);
    mode = AM_MANUAL;
    desired_altitude = altitude;
    if (!TheAltitude->good)
        ThrowException (NO_ALTITUDE);

    this->climb_airspeed = climb_airspeed;
    this->descent_airspeed = descent_airspeed;

    if (pitch_engaged == FALSE)
    {
        calls_to_pitch = pitch_duty_cycle;  // For a pitch update right away
        pitch_engaged = TRUE;
        compute_initial_pitch ();
    }

    hw->servo_state_change (FALSE, FALSE, TRUE, TRUE, FALSE, FALSE);
}

/**
 * SetILS
 * DESCRIPTION:     Couple the autopilot to the ILS
 * PRE-CONDITIONS:  The aircraft is flying, the AHRS has good data, and the ILS or
 *                  localizer has good data.
 * POST-CONDITIONS: Force and trim variables updated, autopilot is engaged.
 * EXCEPTIONS THROWN:  NO_AHRS, NO_AIRSPEED, NOT_FLYING, NO_SERVOS, NO_CDI, NO_GSI
 * EXCEPTIONS HANDLED: 
 */
void
autopilot::SetILS
(
 unsigned       altitude,       // MDA in feet MSL
 unsigned       descent_airspeed  // The target airspeed for a localizer descent
                                // Set to 0 for ILS
)
{
    if (hw == NULL)
        ThrowException (NO_SERVOS);
    mode = AM_ILS;
    desired_altitude = altitude;

    if (TheNAVNeedles->cdi_good == FALSE)
        ThrowException (NO_CDI);
    if ((TheNAVNeedles->gsi_good == FALSE) && (descent_airspeed == 0))
        ThrowException (NO_GSI);

    desired_airspeed = descent_airspeed;

    if (roll_engaged == FALSE)
    {
        roll_engaged = TRUE;
        compute_initial_roll ();
    }
    if (pitch_engaged == FALSE)
    {
        pitch_engaged = TRUE;
        calls_to_pitch = pitch_duty_cycle;  // For a pitch update right away
        compute_initial_pitch ();
    }
    hw->servo_state_change (TRUE, TRUE, TRUE, TRUE, FALSE, FALSE);
}

/**
 * SetAirspeedLimits
 * DESCRIPTION:     Set the desired airspeed limits that the autopilot will not exceed
 *                  under any circumstances.
 * PRE-CONDITIONS:  
 * POST-CONDITIONS: Limits set.
 * EXCEPTIONS THROWN:
 * EXCEPTIONS HANDLED: 
 */
void
autopilot::SetAirspeedLimits
(
 unsigned       min         , // The airspeed the autopilot will never let the aircraft get
                              // below in knots CAS.
 unsigned       max           // The airspeed the autopilot will never let the aircraft get
                              // above in knots CAS.
)
{
    assert (min < max);
    assert (min > 10);
    assert (max > 40);

    min_airspeed = min;
    max_airspeed = max;
}

/**
 * SetVOR
 * DESCRIPTION:     Track a VOR radial
 * PRE-CONDITIONS:  The aircraft is flying, the AHRS has good data, and the CDI
 *                  has good data.
 * POST-CONDITIONS: Force and trim variables updated, autopilot is engaged.
 * EXCEPTIONS THROWN:  NO_AHRS, NOT_FLYING, NO_SERVOS, NO_CDI
 * EXCEPTIONS HANDLED: 
 */
void
autopilot::SetVOR
(
 unsigned       radial,         // Radial to track 1-360
 unsigned       heading         // Heading to fly 1-360. Must be equal to the radial
                                // number or 180 degress different
)
{
    assert(radial <= 360);
    if (hw == NULL)
        ThrowException (NO_SERVOS);
    if (radial == 360)
        assert((heading == 180) || (heading == 360));
    else
        assert((heading == radial) || ((heading + 180) % 360 == radial));
    desired_heading = heading;
    desired_radial = radial;
    mode = AM_VOR;
    if (roll_engaged == FALSE)
    {
        roll_engaged = TRUE;
        compute_initial_roll ();
    }
    hw->servo_state_change (TRUE, TRUE, FALSE, FALSE, FALSE, FALSE);
}

/**
 * Update
 * DESCRIPTION:     Update servo commands to track to set set course
 *                  When engaged, this function should be called often enough to keep
 *                  the autopilot from drifting off course.
 * PRE-CONDITIONS:  One or more servos engaged, flying, valid sensor data
 * POST-CONDITIONS: Servos have latest corrections set in.
 * EXCEPTIONS THROWN:   NO_SERVOS, NOT_FLYING, NO_AHRS, NO_CDI, NO_GSI
 * EXCEPTIONS HANDLED:
 *                                                                                  */
void autopilot::Update (void)
{
    float               dt;     // seconds since last update
    unsigned            curtime;
    float               force_error;
    static int          i = 0;

    if (hw == NULL)
        ThrowException (NO_SERVOS);

    /****  Record the current time to comput delta time between updates *****/
    curtime = time_in_us ();
    if (last_update == 0)
        dt = 1.0 / 1000000.0;
    else
        dt = (curtime - last_update) / 1000000.0;
    last_update = curtime;
    if (last_update == 0)
        last_update = 1;

    if (roll_engaged)
    {
        float   heading_error;
        float   target_roll_angle;
        float   roll_angle_error;
        float   target_roll_angle_prime;
        float   roll_angle_prime_error;

        if (TheAHRS->good == FALSE)
            ThrowException (NO_AHRS);
        switch (mode)
        {
            case AM_ILS:
            case AM_VOR:
            {
                float   desired_cdi_prime, cdi_prime_error;
                if (TheNAVNeedles->cdi_good == FALSE)
                    ThrowException (NO_CDI);
                desired_cdi_prime = cdi_prime_amplifier * TheNAVNeedles->cdi;
                cdi_prime_error = desired_cdi_prime - TheNAVNeedles->cdi_prime;
                ils_desired_heading += cdi_heading_amplifier * cdi_prime_error * dt;
                if (ils_desired_heading < 0)
                    ils_desired_heading += 360;
                else if (ils_desired_heading > 360)
                    ils_desired_heading -= 360;
                // Now that desired heading is computed, fall through to manual
                desired_heading = (int) roundf (ils_desired_heading);
            }
            case AM_MANUAL:
                heading_error = (M_PI * desired_heading / 180) - TheAHRS->heading_angle;
                break;
        }
        // Normalize heading error
        if (heading_error < -M_PI)
            heading_error += 2*M_PI;
        if (heading_error > M_PI)
            heading_error -= 2*M_PI;

        // First, compute desired roll angle from heading error
        // roll_angle_amplifier may change to be a function of ground speed
        // in order to produce standard rate turns
        target_roll_angle = roll_angle_amplifier * heading_error;
        LIMIT_VARIABLE(target_roll_angle,min_roll_angle,max_roll_angle);

        // Second, compute desired roll rate from desired roll angle and actual
        roll_angle_error = target_roll_angle - TheAHRS->roll_angle;
        target_roll_angle_prime = roll_angle_prime_amplifier * roll_angle_error;
        LIMIT_VARIABLE(target_roll_angle_prime,min_roll_angle_prime,max_roll_angle_prime);
        roll_angle_prime_error = target_roll_angle_prime - TheAHRS->roll_angle_prime;

        // Last, compute aileron force/deflection from desired roll rate and actual
        force_error = roll_force_amplifier * roll_angle_prime_error * dt;
        aileron_force += force_error;
        LIMIT_VARIABLE(aileron_force,min_aileron_force,max_aileron_force);
        hw->update_aileron_servo ((int) roundf(aileron_force), 0);
        //printf ("aileron = %f, ", aileron_force);
    }

    if (pitch_engaged)
    {
        float   pitch_error, target_pitch_prime;
        if (TheAirspeed->good == FALSE)
            ThrowException (NO_AIRSPEED);
        if (TheAltitude->good == FALSE)
            ThrowException (NO_ALTITUDE);
        if (TheAHRS->good == FALSE)
            ThrowException (NO_AHRS);
        if (TheAltitude->alt > (int)desired_altitude)
            desired_airspeed = descent_airspeed;
        else
            desired_airspeed = climb_airspeed;
        switch (mode)
        {
            case AM_ILS:
            if (desired_airspeed == 0)
            {
                float   desired_gsi_prime, gsi_prime_error;
                // TODO: keep airspeed within limits
                if (TheNAVNeedles->gsi_good == FALSE)
                    ThrowException (NO_GSI);
                desired_gsi_prime = gsi_prime_amplifier * TheNAVNeedles->gsi;
                gsi_prime_error = desired_gsi_prime - TheNAVNeedles->gsi_prime;
                pitch_error = gsi_pitch_amplifier * gsi_prime_error * dt;
                break;
            }
            // If desired airspeed is non 0, this is a localizer approach and we
            // should descend to MDA at the descent airspeed, so just fall through to
            // manual pitch control
            case AM_VOR:
            case AM_MANUAL:
                int             working_airspeed = desired_airspeed;
                // Find whether the primary instrument is AI or airspeed
                if ((abs(desired_altitude - TheAltitude->alt) > 100) &&
                    (abs(desired_airspeed - TheAirspeed->as) < 10))
                {
control_airspeed:
                    int         airspeed_error;
                    float       desired_airspeed_prime, airspeed_prime_error;
                    // Primary instrument is airspeed indicator
                    if (++calls_to_pitch >= pitch_duty_cycle)
                    {
                        calls_to_pitch = 0;
                        airspeed_error = working_airspeed - TheAirspeed->as;
                        desired_airspeed_prime = as_prime_amplifier * airspeed_error;
                        airspeed_prime_error = desired_airspeed_prime - TheAirspeed->as_prime;
                        desired_pitch += as_pitch_amplifier * airspeed_prime_error * dt * pitch_duty_cycle;
                    }
                } else {
                    int         altitude_error;
                    float       desired_altitude_prime, altitude_prime_error;
                    // Primary instrument is altimeter

                    // But first, check if we're in airspeed limits
                    if (TheAirspeed->as < min_airspeed)
                    {
                        working_airspeed = min_airspeed;
                        goto control_airspeed;
                    } else if (TheAirspeed->as > max_airspeed)
                    {
                        working_airspeed = max_airspeed;
                        goto control_airspeed;
                    }

                    altitude_error = (desired_altitude - TheAltitude->alt);
                    desired_altitude_prime = alt_prime_amplifier * altitude_error;
                    LIMIT_VARIABLE(desired_altitude_prime,min_vsi,max_vsi);

                    pitch_error = target_pitch_prime - TheAHRS->pitch_angle_prime;
                    if ((++calls_to_pitch >= pitch_duty_cycle) &&
                        (abs(pitch_error) < 2 * M_PI / 180))
                    {
                        calls_to_pitch = 0;
                        altitude_prime_error = desired_altitude_prime - TheAltitude->alt_prime;
                        desired_pitch += alt_pitch_amplifier *
                                        altitude_prime_error * dt * pitch_duty_cycle;
                    }
                }
                break;
        }
        LIMIT_VARIABLE(desired_pitch,
                       min_pitch_angle,max_pitch_angle);
        target_pitch_prime = pitch_prime_amplifier * (desired_pitch - TheAHRS-> pitch_angle);
        
        pitch_error = target_pitch_prime - TheAHRS->pitch_angle_prime;
        //if ((i++ % 50) == 0)
        //    printf ("desired_pitch = %f, actual pitch = %f, tpap = %f, pap = %f\n",
        //            desired_pitch * 180 / M_PI,
        //            TheAHRS->pitch_angle * 180 / M_PI,
        //            target_pitch_prime,
        //            TheAHRS->pitch_angle_prime);
        force_error = pitch_error * pitch_force_amplifier * dt;
        //printf ("desired_pitch = %f, force_error = %f\n",
        //        180 * desired_pitch / M_PI,
        //        force_error);
        elevator_force += force_error;
        LIMIT_VARIABLE(elevator_force,min_elevator_force,max_elevator_force);
        hw->update_elevator_servo ((int) roundf(elevator_force), 0);
    }
    if (rudder_engaged)
    {
        if (TheAHRS->good == FALSE)
            ThrowException (NO_AHRS);
        rudder_force += rudder_force_amplifier * TheAHRS->yaw_angle;
        LIMIT_VARIABLE(rudder_force,min_rudder_force,max_rudder_force);
        hw->update_rudder_servo ((int) roundf(rudder_force), 0);
        //printf ("rudder = %f, ", rudder_force);
    }
    //if ((pitch_engaged) || (roll_engaged) || (rudder_engaged))
    //    putchar ('\n');
}

/**
 * EnableAutoCoordination
 * DESCRIPTION:     Turn on rudder control for autocoordination.
 * PRE-CONDITIONS:  Flying with valid AHRS data. Rudder servo available.
 * POST-CONDITIONS: Auto coordination on and active
 * EXCEPTIONS THROWN:   NO_AHRS, NOT_FLYING, NO_SERVOS
 * EXCEPTIONS HANDLED: 
 */
void autopilot::EnableAutoCoordination (void)
{
    if (rudder_engaged == FALSE)
    {
        rudder_engaged = TRUE;
        compute_initial_rudder();
    }
    if (hw == NULL)
        ThrowException (NO_SERVOS);
    hw->servo_state_change (FALSE, FALSE, FALSE, FALSE, TRUE, TRUE);
}

/**
 * DisableAutoCoordination
 * DESCRIPTION:     Turn *off* rudder control for autocoordination.
 * PRE-CONDITIONS:  
 * POST-CONDITIONS: Auto coordination off
 * EXCEPTIONS THROWN:   NO_SERVOS
 * EXCEPTIONS HANDLED: 
 */
void autopilot::DisableAutoCoordination (void)
{
    rudder_engaged = FALSE;
    if (hw == NULL)
        ThrowException (NO_SERVOS);
    hw->servo_state_change (FALSE, FALSE, FALSE, FALSE, TRUE, FALSE);
}

/**
 * compute_initial_roll
 * DESCRIPTION:     Compute initial settings for roll servo before engaging
 * PRE-CONDITIONS:  NO_AHRS, NOT_FLYING, NO_SERVOS
 * POST-CONDITIONS: servo variables updated.
 * EXCEPTIONS THROWN:   NO_SERVOS
 * EXCEPTIONS HANDLED: 
 */
void
autopilot::compute_initial_roll (void)
{
    bool        pitch_engaged_org, rudder_engaged_org;

    pitch_engaged_org = pitch_engaged;
    rudder_engaged_org = rudder_engaged;
    switch (mode)
    {
        case AM_ILS:
            ils_desired_heading = TheAHRS->heading_angle * 180 / M_PI;
            break;
        case AM_VOR:
            ils_desired_heading = desired_heading;
            break;
        default:
            break;
    }
    // Set last update to a while ago so they autopilot will update more quickly initially
    // Pretend the last update was 1 seconds ago
    // last_update = time_in_us() - 100000;
    // Center the ailerons
    aileron_force = 0;
    Update();
    pitch_engaged = pitch_engaged_org;
    rudder_engaged = rudder_engaged_org;
}

/**
 * compute_initial_pitch
 * DESCRIPTION:     Compute initial settings for pitch servo before engaging
 * PRE-CONDITIONS:  NO_AHRS, NOT_FLYING, NO_SERVOS
 * POST-CONDITIONS: servo variables updated.
 * EXCEPTIONS THROWN:   NO_SERVOS
 * EXCEPTIONS HANDLED: 
 */
void
autopilot::compute_initial_pitch (void)
{
    bool        roll_engaged_org, rudder_engaged_org;

    roll_engaged_org = roll_engaged;
    rudder_engaged_org = rudder_engaged;
    // Set last update to a while ago so they autopilot will update more quickly initially
    // Pretend the last update was 10 seconds ago
    //last_update = time_in_us() - 10000000;

    // Center the elevator
    elevator_force = 0;

    Update();
    roll_engaged = roll_engaged_org;
    rudder_engaged = rudder_engaged_org;
}

/**
 * compute_initial_rudder
 * DESCRIPTION:     Compute initial settings for rudder servo before engaging
 * PRE-CONDITIONS:  NO_AHRS, NOT_FLYING, NO_SERVOS
 * POST-CONDITIONS: servo variables updated.
 * EXCEPTIONS THROWN:   NO_SERVOS
 * EXCEPTIONS HANDLED: 
 */
void
autopilot::compute_initial_rudder (void)
{
    bool        roll_engaged_org, pitch_engaged_org;

    roll_engaged_org = roll_engaged;
    pitch_engaged_org = pitch_engaged;
    // Set last update to a while ago so they autopilot will update more quickly initially
    // Pretend the last update was 10 seconds ago
    // last_update = time_in_us() - 10000000;

    // Center the rudder
    rudder_force = 0;
    Update();
    roll_engaged = roll_engaged_org;
    pitch_engaged = pitch_engaged_org;
}

/**
 * servo_state_change
 * DESCRIPTION:     For three servos, turn on, off, or leave in previous state
 * PRE-CONDITIONS:  
 * POST-CONDITIONS: 
 * EXCEPTIONS THROWN:   NO_SERVOS
 * EXCEPTIONS HANDLED: 
 */
void autopilot_hardware::servo_state_change (
    bool            roll_change,            // TRUE if servo state should change
    bool            roll_state,             // TRUE if servo should be on.
                                            // not used if "change" variable is false.
    bool            pitch_change,           // TRUE if servo state should change
    bool            pitch_state,            // TRUE if servo should be on.
                                            // not used if "change" variable is false.
    bool            rudder_change,          // TRUE if servo state should change
    bool            rudder_state            // TRUE if servo should be on.
                                            // not used if "change" variable is false.
)
{
    if (roll_change)
        aileron_servo_on = roll_state;

    if (pitch_change)
        elevator_servo_on = pitch_state;

    if (rudder_change)
        rudder_servo_on = rudder_state;
}

autopilot::autopilot(void)
{
    // Amplifiers: digital correlaries to analog amplifiers which are integral
    // to the autopilot control system.
    roll_angle_amplifier            = 1.5; // 10 degrees off course, bank 15 degrees
    roll_angle_prime_amplifier      = 0.5;
    // roll_force_amplifier must have units of:
    // percent force / ((radians / s) * s)
    // If one second goes by and the current aileron force produces an roll rate
    // error of 10 degrees per second, adjust the percent force by 30 percent?
    // This value is empirically derived.
    // 30 / (10 * M_PI / 180)
    roll_force_amplifier            = 2.0 / (M_PI / 180);

    // For ILS/VOR
    cdi_prime_amplifier     = .2;   // 1/5 degree/s for every degree off
    // change 1/2 degree per second for every degree/s the cdi should change
    cdi_heading_amplifier   = .5;
    // For ILS pitch
    gsi_prime_amplifier     = .2;
    gsi_pitch_amplifier     = 1 / (2 * M_PI);

    // For pitch
    as_prime_amplifier      = 1;
    // For every 1 knot away from the desired airspeed, increase pitch by
    // .05 degrees per second
    as_pitch_amplifier      = .05 * M_PI / 180.0;
    // To change the pitch 1 degree per sec, add .5% of total force per second
    pitch_force_amplifier   = 5 / (1 * M_PI / 180);
    alt_prime_amplifier     = 1.5;
    // If you want 2000 feet per minute, add 5 degrees pitch per 15 sec
    alt_pitch_amplifier     = (5 * M_PI / 180) / (2000.0 * 10);
    rudder_force_amplifier  = -1 * M_PI / 180;
    // For an error of 5 degrees of pitch, move toward target pitch at 1 degree per second
    pitch_prime_amplifier   = 1.0/5.0;
    pitch_duty_cycle        = 3;

    // Limits: Correlaries to the amplifier voltage limits
    min_roll_angle          = M_PI * -35 / 180;
    max_roll_angle          = M_PI *  35 / 180;
    min_pitch_angle         = M_PI * -10 / 180;
    max_pitch_angle         = M_PI *  10 / 180;
    min_roll_angle_prime    = M_PI * -10 / 180;
    max_roll_angle_prime    = M_PI *  10 / 180;
    min_aileron_force       = -100;
    max_aileron_force       = 100;
    min_elevator_force      = -100;
    max_elevator_force      = 100;
    min_vsi                 = -1000;
    max_vsi                 = 1000;
    min_rudder_force        = -100;
    max_rudder_force        = 100;
    roll_engaged = pitch_engaged = rudder_engaged = FALSE;
}

/**
 * update_aileron_servo
 * DESCRIPTION:     Send a new setting to the aileron servo
 * PRE-CONDITIONS:  
 * POST-CONDITIONS: 
 * EXCEPTIONS THROWN:   NO_SERVOS
 * EXCEPTIONS HANDLED: 
 */
void autopilot_hardware::update_aileron_servo (
    int         new_value,              // new position / force to set the servo to
    int         new_trim_value          // new position / force to trim to
                                        // Both arguments range between +/- 100%
)
{
    ThrowException (NO_SERVOS);
}

/**
 * update_elevator_servo
 * DESCRIPTION:     Send a new setting to the elevator servo
 * PRE-CONDITIONS:  
 * POST-CONDITIONS: 
 * EXCEPTIONS THROWN:   NO_SERVOS
 * EXCEPTIONS HANDLED: 
 */
void autopilot_hardware::update_elevator_servo (
    int         new_value,              // new position / force to set the servo to
    int         new_trim_value          // new position / force to trim to
                                        // Both arguments range between +/- 100%
)
{
    ThrowException (NO_SERVOS);
}


/**
 * update_rudder_servo
 * DESCRIPTION:     Send a new setting to the rudder servo
 * PRE-CONDITIONS:  
 * POST-CONDITIONS: 
 * EXCEPTIONS THROWN:   NO_SERVOS
 * EXCEPTIONS HANDLED: 
 */
void autopilot_hardware::update_rudder_servo (
    int         new_value,              // new position / force to set the servo to
    int         new_trim_value          // new position / force to trim to
                                        // Both arguments range between +/- 100%
)
{
    ThrowException (NO_SERVOS);
}

autopilot_hardware::autopilot_hardware (void)
{
    actual_aileron_force = 0;
    actual_elevator_force = 0;
    actual_rudder_force = 0;

    aileron_servo_on = 0;
    elevator_servo_on = 0;
    rudder_servo_on = 0;

    actual_aileron_trim = 0;
    actual_elevator_trim = 0;
    actual_rudder_trim = 0;

    io_board_fd = 0;            // File descriptor for I/O board
    servo_scale = 1.0;
}

