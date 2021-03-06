// test_aircraft.h: Class definition of primary class to simulate aircraft behavior for
//                      testing the EFIS system
//

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


class aircraft
{
    public:
        // Flight parameters
        double          pitch_angle;            // In radians
        double          dpitch_angle;           // 1st deriviative In radians/s
        double          roll_angle;             // In radians
        double          droll_angle;            // 1st deriviative In radians/s
        double          yaw_angle;              // In radians
        double          dyaw_angle;             // 1st deriviative In radians/s
        unsigned        airspeed;               // In knots CAS
        unsigned        dairspeed;              // 1st deriviative In knots CAS/s
        unsigned        altitude;               // In feet MSL
        unsigned        daltitude;              // 1st deriviative In feet MSL/s
        unsigned        heading;                // 1-360 compass heading
        unsigned        dheading;               // 1st deriviative 1-360 degrees / s
        double          lat, lng;               // GPS coordinates

    protected:
        // Aircraft configuration
        double          aileron_position;       // Right roll deflection in radians
        double          elevator_position;      // Pitch up deflection in radians
        double          rudder_position;        // Yaw right deflection in radians
        double          flap_position;          // down deflection in radians
        unsigned        engine_power;           // Percent power output of engine(s)
        // Landing gear ignored for now

        // Environmental parameters
        int             oat;                    // Outside air temp in degrees C
        double          altimeter;              // Barametric pressure in "Hg
        unsigned        wa_heading;             // Winds aloft
        unsigned        wa_speed;               // Winds aloft speed
        unsigned        turbulence;             // Amount of turbulence on a scale of 1-100
        unsigned        density_altitude;       // Density altitude (computed and saved)

    public:
        // Inputs from test or autopilot

        /**
         * SetAilerons
         * DESCRIPTION:     Set new aileron input to the aircraft
         * PRE-CONDITIONS:  
         * POST-CONDITIONS: Ailerons set
         * EXCEPTIONS THROWN:  None
         * EXCEPTIONS HANDLED: None
         */
        void SetAilerons
        (
         double         deflection              // Right roll deflection in radians
        );

        /**
         * SetElevator
         * DESCRIPTION:     Set new elevator input to the aircraft
         * PRE-CONDITIONS:  
         * POST-CONDITIONS: Elevator set
         * EXCEPTIONS THROWN:  None
         * EXCEPTIONS HANDLED: None
         */
        void SetElevator
        (
         double         deflection              // Pitch up deflection in radians
        );

        /**
         * SetRudder
         * DESCRIPTION:     Set new rudder input to the aircraft
         * PRE-CONDITIONS:  
         * POST-CONDITIONS: Rudder set
         * EXCEPTIONS THROWN:  None
         * EXCEPTIONS HANDLED: None
         */
        void SetRudder
        (
         double         deflection              // Right yaw deflection in radians
        );

        /**
         * SetFlaps
         * DESCRIPTION:     Set new flaps input to the aircraft
         * PRE-CONDITIONS:  
         * POST-CONDITIONS: Flaps set
         * EXCEPTIONS THROWN:  None
         * EXCEPTIONS HANDLED: None
         */
        void SetFlaps
        (
         double         deflection              // Downward deflection in radians
        );

        /**
         * SetPower
         * DESCRIPTION:     Set power level to the aircraft
         * PRE-CONDITIONS:  
         * POST-CONDITIONS: Power set
         * EXCEPTIONS THROWN:  None
         * EXCEPTIONS HANDLED: None
         */
        void SetPower
        (
         unsigned       power_setting           // Percent power output of engine(s)
        );

        /**
         * IncrementTime
         * DESCRIPTION:     Simulates the aircraft and computes its state for the given time
         *                  period.
         * PRE-CONDITIONS:  Environmental factors loaded.
         * POST-CONDITIONS: Aircraft state variables advanced
         * EXCEPTIONS THROWN:  NO_ENVIRONMENT
         * EXCEPTIONS HANDLED: None
         */
        void IncrementTime
        (
         double         seconds         // How many seconds to advance the state of the aircraft
        );

        /**
         * ReadInitialConditions
         * DESCRIPTION:     Read aircraft and environment initial conditions
         * PRE-CONDITIONS:  
         * POST-CONDITIONS: Class initialized and aircraft ready to simulate
         * EXCEPTIONS THROWN:  NO_SUCH_FILE
         * EXCEPTIONS HANDLED: None
         */
        SyntaxError     *       // Pointer to syntax error descriptor if there was an error
                                // in the file. NULL on success. Calling function must free
                                // pointer.
        ReadInitialConditions
        (
         string         path
        );
}
