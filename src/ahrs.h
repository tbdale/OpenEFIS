// ahrs.h: Class definition for the attitude heading reference system
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


#ifndef AHRS_H
#define AHRS_H

#include "syntax_error.h"

// Hardware abstraction class:
class ahrs_hardware;

// Behavior abstraction class:
// This parent class assumes raw e-gyro and accelerometer data.
// If the actual sensor presents pre-cooked data, override the comput_* functions
// to account for this.
class ahrs
{
    public:
        // All the following angles are in radians
        bool            good;

        float           roll_angle;
        float           pitch_angle;
        float           heading_angle;
        float           yaw_angle;

        float           roll_angle_prime;       // Change in roll angle r/s
        float           pitch_angle_prime;      // Change in pitch angle r/s
        float           heading_angle_prime;    // Change in heading angle r/s
        float           yaw_angle_prime;        // Change in yaw angle r/s

        unsigned        duty_cycle;

    protected:
        // noise_constant = some value greater than drift introduced by random noise.
        //                  for angular rate sensors.
        // noise_constant = radians / s max drift towards estimated value
        float           noise_constant;
        // yaw_roll_constant: The amount uncoordinated flight can contribute to heading
        // changes to provide the appearance of or the absense of roll.
        // yaw_roll_constant = radians / unit sampled from yaw accelerometer
        float           yaw_roll_constant;

        ahrs_hardware  *data_source;

    public:
        ahrs(void);

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
             ahrs_hardware *ashw
            )
            {data_source = ashw;}

        /**
         * SampleAndCompute
         * DESCRIPTION:     Sample raw sensor data and compute filtered data
         * PRE-CONDITIONS:  The aircraft is flying, not taxiing.
         *                  Call SampleAndComputeTaxi when aircraft is on the ground.
         *                  io_board_fd is open to a valid device driver.
         * POST-CONDITIONS: good flag is FALSE, or all public angle data is correct.
         * EXCEPTIONS THROWN:  NO_IO_BOARD, BAD_IO_DRIVER
         * EXCEPTIONS HANDLED: None
         */
        void SampleAndCompute (void);
        /**
         * SampleAndComputeStill
         * DESCRIPTION:     Sample raw sensor data and compute filtered data assuming the aircraft
         *                  is on the ground.
         * PRE-CONDITIONS:  The aircraft is taxiing, not flying.
         *                  Call SampleAndCompute when aircraft is flying.
         *                  io_board_fd is open to a valid device driver.
         * POST-CONDITIONS: good flag is FALSE, or all public angle data is correct.
         * EXCEPTIONS THROWN:  NO_IO_BOARD, BAD_IO_DRIVER
         * EXCEPTIONS HANDLED: None
         */
        void SampleAndComputeStill (void);

        /**
         * InitConstants
         * DESCRIPTION:     Read sensor constants from a file.
         * PRE-CONDITIONS:  The given path exists and is in the correct format.
         * POST-CONDITIONS: sensor constants initialized
         * EXCEPTIONS THROWN:  NO_SUCH_FILE
         * EXCEPTIONS HANDLED: None
         */
        SyntaxError *
        InitConstants (const char *path);

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
         unsigned    main_loop_interval  // The period in uS of the main loop
        );

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

extern ahrs    *TheAHRS;

// Hardware abstraction class:
class ahrs_hardware
{
    public:
        // Raw sensor data.
        float           accel_thrust;   // Accelerometer data in m/s^2
        float           accel_yaw;
        float           accel_lift;

        float           ang_roll;       // ang rate data in radians/s
        float           ang_pitch;
        float           ang_head;

        float           roll_cooked;    // Absolute angle data in radians
        float           pitch_cooked;
        float           heading_cooked;

        bool            good;

        unsigned        dt;             // Delta time in microseconds since
                                        // the last valid sample. == 1 if there
                                        // was no previously good sample.

        /**
         * Sample
         * DESCRIPTION:     Sample sensor data.
         * PRE-CONDITIONS:  io_board_fd is open to a valid device driver.
         * POST-CONDITIONS: good flag is FALSE, or all public angle data is correct.
         * EXCEPTIONS THROWN:  NO_IO_BOARD, BAD_IO_DRIVER
         * EXCEPTIONS HANDLED: None
         */
        virtual bool    // Returns TRUE if new data was available
                        // or FALSE if the data remains unchanged as a result
                        // of calling this function.
            Sample (void);

        ahrs_hardware ();

    protected:
        // File descriptor open to I/O board
        int             io_board_fd;
        float           ang_scale;
        float           accel_scale;
};

#endif
