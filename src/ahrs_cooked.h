// ahrs_cooked.h: Class definition for the attitude heading reference system
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

// Behavior abstraction class:
// This is a child class to "ahrs"
class ahrs_cooked : public ahrs
{
    public:
        ahrs_cooked(void);

    protected:

        /**
         * compute_pitch
         * DESCRIPTION:     Compute the pitch by integrating angular rate sensor data and
         *                  filtering it with data from the accelerometers
         * PRE-CONDITIONS:  raw sensor data current.
         * POST-CONDITIONS: pitch_angle updated
         * EXCEPTIONS THROWN:  
         * EXCEPTIONS HANDLED: 
         */
        virtual void
        compute_pitch
        (
         unsigned       dt,     // Delta time to use when integrating data. Set to zero
                                // when taxiing to not use angular rate sensor data.
                                // Units in microseconds.
         float          dv      // Delta velocity in knots to filter pitch with.
                                // Comes from GPS when flying or the wheel speed when taxiing.
        );


        /**
         * compute_roll_flying
         * DESCRIPTION:     Compute the roll angle
         * PRE-CONDITIONS:  The aircraft is flying
         * PRE-CONDITIONS:  raw sensor data current.
         * POST-CONDITIONS: roll_angle is accurate
         * EXCEPTIONS THROWN:  
         * EXCEPTIONS HANDLED: 
         */
        virtual void
        compute_roll_flying
        (
         unsigned       dt      // Delta time to use when integrating data.
                                // Units in microseconds.
        );

        /**
         * compute_roll_still
         * DESCRIPTION:     Compute the roll angle based entirely on the g angle from accelerometer data
         * PRE-CONDITIONS:  The aircraft is stationary on the ground
         * PRE-CONDITIONS:  raw sensor data current.
         * POST-CONDITIONS: roll_angle is accurate
         * EXCEPTIONS THROWN:  
         * EXCEPTIONS HANDLED: 
         */
        virtual void
        compute_roll_still
        (void);

        /**
         * compute_heading_flying
         * DESCRIPTION:     Compute the heading angle of aircraft as slaved to the compass
         * PRE-CONDITIONS:  The aircraft is flying
         * PRE-CONDITIONS:  raw sensor data current.
         * POST-CONDITIONS: heading_angle is accurate
         * EXCEPTIONS THROWN:  
         * EXCEPTIONS HANDLED: 
         */
        virtual void
        compute_heading_flying
        (
         unsigned       dt      // Delta time to use when integrating data.
                                // Units in microseconds.
        );

        /**
         * compute_heading_still
         * DESCRIPTION:     Compute the heading angle of aircraft as slaved to the compass
         * PRE-CONDITIONS:  The aircraft is stationary on the ground
         * PRE-CONDITIONS:  raw sensor data current.
         * POST-CONDITIONS: heading_angle is accurate
         * EXCEPTIONS THROWN:  
         * EXCEPTIONS HANDLED: 
         */
        virtual void
        compute_heading_still
        (void);

        /**
         * compute_yaw
         * DESCRIPTION:     Compute yaw (slip/skid) based on accelerometer data
         * PRE-CONDITIONS:  The aircraft is stationary on the ground
         * PRE-CONDITIONS:  raw sensor data current.
         * POST-CONDITIONS: yaw_angle is accurate
         * EXCEPTIONS THROWN:  
         * EXCEPTIONS HANDLED: 
         */
        virtual void
        compute_yaw
        (void);

};
