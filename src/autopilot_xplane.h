// autopilot_xplane.h: Class definition for the autopilot servo for X-Plane simulation
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

#include "autopilot.h"

class autopilot_xplane : public autopilot_hardware
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

        autopilot_xplane (void);

    protected:
        float           servo_scale;
        float           xplane_scale;

        /**
         * send_control_info
         * DESCRIPTION:     Send the updated flight control settings to X-Plane via UDP
         * PRE-CONDITIONS:  
         * POST-CONDITIONS: 
         * EXCEPTIONS THROWN:   NO_IO_BOARD
         * EXCEPTIONS HANDLED: 
         */
        virtual void send_control_info (void);
};
