
// autopilot.h: Class definition for the autopilot
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

#include "exceptions.h"

typedef enum en_autopilot_mode {
    AM_MANUAL,
    AM_VOR,
    AM_ILS
} AutopilotMode;

// Hardware (driver) abstraction class
class autopilot_hardware;

class autopilot
{
    public:
        bool    roll_engaged;
        bool    pitch_engaged;
        bool    rudder_engaged; // Yaw control engages seperately from roll and pitch

        unsigned duty_cycle;

        autopilot (void);

        /**
         * Disengage
         * DESCRIPTION:     Disengage autopilot servos from controls
         * PRE-CONDITIONS:
         * POST-CONDITIONS: Servos inactive, autopilot stopped
         * EXCEPTIONS THROWN: NO_SERVOS
         * EXCEPTIONS HANDLED: 
         */
        void Disengage (void);

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
        SetHeading
        (
         unsigned       heading         // New heading in degrees 1-360
        );
        /**
         * SetAltitude
         * DESCRIPTION:     Set the desired altitude for the autopilot
         * PRE-CONDITIONS:  The aircraft is flying and the AHRS has good data.
         * POST-CONDITIONS: Force and trim variables updated, autopilot is engaged.
         * EXCEPTIONS THROWN:  NO_ALTITUDE, NO_AIRSPEED, NOT_FLYING, NO_SERVOS
         * EXCEPTIONS HANDLED: 
         */
        void
        SetAltitude
        (
         unsigned       altitude,       // New altitude in feet MSL
         unsigned       climb_airspeed, // The target airspeed for a protracted climb
         unsigned       descent_airspeed  // The target airspeed for a protracted descent
        );

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
        SetILS
        (
         unsigned       altitude,       // MDA in feet MSL
         unsigned       descent_airspeed  // The target airspeed for a localizer descent
                                        // Set to 0 for ILS
        );

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
        SetVOR
        (
         unsigned       radial,         // Radial to track 1-360
         unsigned       heading         // Heading to fly 1-360. Must be equal to the radial
                                        // number or 180 degress different
        );

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
        SetAirspeedLimits
        (
         unsigned       min_airspeed, // The airspeed the autopilot will never let the aircraft get
                                      // below in knots CAS.
         unsigned       max_airspeed  // The airspeed the autopilot will never let the aircraft get
                                      // above in knots CAS.
        );

        /**
         * EnableAutoCoordination
         * DESCRIPTION:     Turn on rudder control for autocoordination.
         * PRE-CONDITIONS:  Flying with valid AHRS data. Rudder servo available.
         * POST-CONDITIONS: Auto coordination on and active
         * EXCEPTIONS THROWN:   NO_AHRS, NOT_FLYING, NO_SERVOS
         * EXCEPTIONS HANDLED: 
         */
        void EnableAutoCoordination (void);

        /**
         * DisableAutoCoordination
         * DESCRIPTION:     Turn *off* rudder control for autocoordination.
         * PRE-CONDITIONS:  
         * POST-CONDITIONS: Auto coordination off
         * EXCEPTIONS THROWN:   NO_SERVOS
         * EXCEPTIONS HANDLED: 
         */
        void DisableAutoCoordination (void);

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
             autopilot_hardware *aphw
            )
            {hw = aphw;}

        /**
         * Update
         * DESCRIPTION:     Update servo commands to track to set set course
         *                  When engaged, this function should be called often enough to keep
         *                  the autopilot from drifting off course.
         * PRE-CONDITIONS:  One or more servos engaged, flying, valid sensor data
         * POST-CONDITIONS: Servos have latest corrections set in.
         * EXCEPTIONS THROWN:   NO_SERVOS, NOT_FLYING, NO_AHRS
         * EXCEPTIONS HANDLED: 
         */
        void Update (void);

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
        ) {return 300000 / main_loop_interval;} // Call ~ every 1/3 sec

    protected:
        float   aileron_force;          // Percent force/deflection +/- 100
        float   elevator_force;         // Percent force/deflection +/- 100
        float   rudder_force;           // Percent force/deflection +/- 100

        float   aileron_trim;           // Percent force/deflection +/- 100
        float   elevator_trim;          // Percent force/deflection +/- 100
        float   rudder_trim;            // Percent force/deflection +/- 100

        autopilot_hardware     *hw;     // The hardware abstraction class used

        unsigned        min_airspeed;           // In KIAS
        unsigned        max_airspeed;           // In KIAS
        unsigned        climb_airspeed;         // In KIAS
        unsigned        descent_airspeed;       // In KIAS

        int             desired_altitude;       // In feet MSL
        unsigned        desired_heading;        // In degrees 1-360
        float           ils_desired_heading;    // Also in degrees, but can change smoothly
        unsigned        desired_radial;         // In degrees 1-360
        unsigned        desired_airspeed;       // In KIAS
        float           desired_pitch;          // Computed. In radians
        float           desired_pitch_offset;   // Control varialbe. In radians

        int             last_update;            // Last update time in microseconds
        int             pitch_duty_cycle;       // The number of update calls
                                                // that should go by between
                                                // updating the pitch angle
                                                // according to vsi or as.
                                                // This is a positive number
                                                // because the aircraft pitch
                                                // and roll react faster than
                                                // vsi, as, and change in heading
        int             calls_to_pitch;         // Counter for pitch_duty_cycle

        AutopilotMode   mode;

        // Amplifiers: digital correlaries to analog amplifiers which are integral
        // to the autopilot control system.
        float     roll_angle_amplifier; // 10 degrees off course, bank 15 degrees
        float     roll_angle_prime_amplifier;
        // roll_force_amplifier must have units of:
        // percent force / ((radians / s) * s)
        // If one second goes by and the current aileron force produces an roll rate
        // error of 10 degrees per second, adjust the percent force by 30 percent?
        // This value is empirically derived.
        // 30 / (10 * M_PI / 180)
        float     roll_force_amplifier;

        // For ILS/VOR
        float     cdi_prime_amplifier;   // 1/5 degree/s for every degree off
        // change 1/2 degree per second for every degree/s the cdi should change
        float     cdi_heading_amplifier;
        // For ILS pitch
        float     gsi_prime_amplifier;
        float     gsi_pitch_amplifier;

        // For pitch
        float     as_prime_amplifier;
        float     as_pitch_amplifier;
        float     pitch_force_amplifier;
        float     alt_prime_amplifier;
        float     alt_pitch_amplifier;
        float     pitch_prime_amplifier;

        // For rudder
        float     rudder_force_amplifier;

        // Limits: Correlaries to the amplifier voltage limits
        float     min_roll_angle;
        float     max_roll_angle;
        float     min_pitch_angle;
        float     max_pitch_angle;
        float     min_roll_angle_prime;
        float     max_roll_angle_prime;
        float     min_aileron_force;
        float     max_aileron_force;
        float     min_elevator_force;
        float     max_elevator_force;
        float     min_vsi;
        float     max_vsi;
        float     min_rudder_force;
        float     max_rudder_force;

        /**
         * compute_initial_roll
         * DESCRIPTION:     Compute initial settings for roll servo before engaging
         * PRE-CONDITIONS:  NO_AHRS, NOT_FLYING, NO_SERVOS
         * POST-CONDITIONS: servo variables updated.
         * EXCEPTIONS THROWN:   NO_SERVOS
         * EXCEPTIONS HANDLED: 
         */
        virtual void
        compute_initial_roll (void);

        /**
         * compute_initial_pitch
         * DESCRIPTION:     Compute initial settings for pitch servo before engaging
         * PRE-CONDITIONS:  NO_AHRS, NOT_FLYING, NO_SERVOS
         * POST-CONDITIONS: servo variables updated.
         * EXCEPTIONS THROWN:   NO_SERVOS
         * EXCEPTIONS HANDLED: 
         */
        virtual void
        compute_initial_pitch (void);

        /**
         * compute_initial_rudder
         * DESCRIPTION:     Compute initial settings for rudder servo before engaging
         * PRE-CONDITIONS:  NO_AHRS, NOT_FLYING, NO_SERVOS
         * POST-CONDITIONS: servo variables updated.
         * EXCEPTIONS THROWN:   NO_SERVOS
         * EXCEPTIONS HANDLED: 
         */
        virtual void
        compute_initial_rudder (void);
};

extern autopilot       *TheAutopilot;

class autopilot_hardware
{
    public:

        /**
         * servo_state_change
         * DESCRIPTION:     For three servos, turn on, off, or leave in previous state
         * PRE-CONDITIONS:  
         * POST-CONDITIONS: 
         * EXCEPTIONS THROWN:   NO_SERVOS
         * EXCEPTIONS HANDLED: 
         */
        virtual void servo_state_change (
            bool            roll_change,            // TRUE if servo state should change
            bool            roll_state,             // TRUE if servo should be on.
                                                    // not used if "change" variable is false.
            bool            pitch_change,           // TRUE if servo state should change
            bool            pitch_state,            // TRUE if servo should be on.
                                                    // not used if "change" variable is false.
            bool            rudder_change,          // TRUE if servo state should change
            bool            rudder_state            // TRUE if servo should be on.
                                                    // not used if "change" variable is false.
        );

        /**
         * update_aileron_servo
         * DESCRIPTION:     Send a new setting to the aileron servo
         * PRE-CONDITIONS:  
         * POST-CONDITIONS: 
         * EXCEPTIONS THROWN:   NO_SERVOS
         * EXCEPTIONS HANDLED: 
         */
        virtual void update_aileron_servo (
            int         new_value,              // new position / force to set the servo to
            int         new_trim_value          // new position / force to trim to
                                                // Both arguments range between +/- 100%
        );

        /**
         * update_elevator_servo
         * DESCRIPTION:     Send a new setting to the elevator servo
         * PRE-CONDITIONS:  
         * POST-CONDITIONS: 
         * EXCEPTIONS THROWN:   NO_SERVOS
         * EXCEPTIONS HANDLED: 
         */
        virtual void update_elevator_servo (
            int         new_value,              // new position / force to set the servo to
            int         new_trim_value          // new position / force to trim to
                                                // Both arguments range between +/- 100%
        );


        /**
         * update_rudder_servo
         * DESCRIPTION:     Send a new setting to the rudder servo
         * PRE-CONDITIONS:  
         * POST-CONDITIONS: 
         * EXCEPTIONS THROWN:   NO_SERVOS
         * EXCEPTIONS HANDLED: 
         */
        virtual void update_rudder_servo (
            int         new_value,              // new position / force to set the servo to
            int         new_trim_value          // new position / force to trim to
                                                // Both arguments range between +/- 100%
        );

        autopilot_hardware (void);

    protected:
        int             actual_aileron_force;
        int             actual_elevator_force;
        int             actual_rudder_force;

        bool            aileron_servo_on;
        bool            elevator_servo_on;
        bool            rudder_servo_on;

        int             actual_aileron_trim;
        int             actual_elevator_trim;
        int             actual_rudder_trim;

        float           servo_scale;

        int             io_board_fd;            // File descriptor for I/O board
};

#define LIMIT_VARIABLE(x,min,max) if ((x) < (min)) (x) = (min); else if ((x) > (max)) (x) = (max);
