// autopilot_xplane.cpp: Class definition for the autopilot servo for X-Plane simulation
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

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <math.h>

#include "xplane_interface.h"
#include "constants.h"

#include "autopilot_xplane.h"

/**
 * update_aileron_servo
 * DESCRIPTION:     Send a new setting to the aileron servo
 * PRE-CONDITIONS:  
 * POST-CONDITIONS: 
 * EXCEPTIONS THROWN:   NO_SERVOS
 * EXCEPTIONS HANDLED: 
 */
void autopilot_xplane::update_aileron_servo
(
  int         new_value,              // new position / force to set the servo to
  int         new_trim_value          // new position / force to trim to
                                      // Both arguments range between +/- 100%
)
{
    if (aileron_servo_on)
    {
        actual_aileron_force = (int) roundf (new_value * servo_scale);
        send_control_info();
    }
}

/**
 * update_elevator_servo
 * DESCRIPTION:     Send a new setting to the elevator servo
 * PRE-CONDITIONS:  
 * POST-CONDITIONS: 
 * EXCEPTIONS THROWN:   NO_SERVOS
 * EXCEPTIONS HANDLED: 
 */
void autopilot_xplane::update_elevator_servo (
    int         new_value,              // new position / force to set the servo to
    int         new_trim_value          // new position / force to trim to
                                        // Both arguments range between +/- 100%
)
{
    if (elevator_servo_on)
    {
        actual_elevator_force = (int) roundf (new_value * servo_scale);
        send_control_info();
    }
}


/**
 * update_rudder_servo
 * DESCRIPTION:     Send a new setting to the rudder servo
 * PRE-CONDITIONS:  
 * POST-CONDITIONS: 
 * EXCEPTIONS THROWN:   NO_SERVOS
 * EXCEPTIONS HANDLED: 
 */
void autopilot_xplane::update_rudder_servo (
    int         new_value,              // new position / force to set the servo to
    int         new_trim_value          // new position / force to trim to
                                        // Both arguments range between +/- 100%
)
{
    if (rudder_servo_on)
    {
        actual_rudder_force = (int) roundf (new_value * servo_scale);
        send_control_info();
    }
}

autopilot_xplane::autopilot_xplane (void)
{
    struct sockaddr_in  lcl;

    ::autopilot_hardware();

    servo_scale = 1.0;
    xplane_scale = 1.0 / 500.0;

    io_board_fd = socket (PF_INET, SOCK_DGRAM, 0);
    if (io_board_fd == -1)
    {
        perror ("autopilot_xplane: socket");
        ThrowException(errno);
    }
    lcl.sin_family = AF_INET;
    lcl.sin_port = htons (48007);
    lcl.sin_addr.s_addr = htonl (INADDR_LOOPBACK);

    bind (io_board_fd, (struct sockaddr*) &lcl, sizeof (lcl));
    fcntl (io_board_fd, F_SETFL, O_NONBLOCK);
}

/**
 * send_control_info
 * DESCRIPTION:     Send the updated flight control settings to X-Plane via UDP
 * PRE-CONDITIONS:  
 * POST-CONDITIONS: 
 * EXCEPTIONS THROWN:   NO_IO_BOARD
 * EXCEPTIONS HANDLED: 
 */
void autopilot_xplane::send_control_info (void)
{
    struct sockaddr_in  xplane;
    static char         xmt_buffer [1500];
    int                 type;
    float               data [8];
    static int          duty_cycle = 0, i = 0;

    if ((duty_cycle++ % 9) == 0)
    {
        xplane.sin_family = AF_INET;
        xplane.sin_port = htons (48006);
        xplane.sin_addr.s_addr = htonl (INADDR_LOOPBACK);

        strcpy (xmt_buffer, "DATA");
        xmt_buffer [5] = '0';

        type = XPT_FLT_CONTROL;

        memcpy (xmt_buffer + 6, (void*)&type, sizeof(type));
        data [0] = actual_elevator_force * xplane_scale;
        data [1] = actual_aileron_force * xplane_scale;
        data [2] = actual_rudder_force * xplane_scale;
        data [3] = 0;
        data [4] = 0;
        data [5] = 0;
        data [6] = 0;
        data [7] = 0;

        if ((i++) % 10 == 0)
            printf ("el = %f, ail = %f, r = %f\n", data [0], data[1], data[2]);

        memcpy (xmt_buffer + 10, data, sizeof (data));

        sendto (io_board_fd, xmt_buffer, 6 + sizeof (type) + sizeof (data),
                0, (struct sockaddr*)&xplane, sizeof (xplane));
    }
}

/**
 * servo_state_change
 * DESCRIPTION:     For three servos, turn on, off, or leave in previous state
 * PRE-CONDITIONS:  
 * POST-CONDITIONS: 
 * EXCEPTIONS THROWN:   NO_SERVOS
 * EXCEPTIONS HANDLED: 
 */
void autopilot_xplane::servo_state_change (
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
    autopilot_hardware::servo_state_change (roll_change, roll_state, pitch_change, pitch_state,
                          rudder_change, rudder_state);
}
