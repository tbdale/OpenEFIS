// test_main_loop.cpp: Test the avionics package using an interface to X-Plane
// Must be running the xplane_interface program in the background on the same machine
// as this program.
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
#include <math.h>

#include <stdlib.h>

#include "utilities.h"
#include "constants.h"
#include "exceptions.h"

#include "ahrs_xplane.h"
#include "ahrs_cooked.h"
#include "airspeed_xplane.h"
#include "gps_xplane.h"
#include "altitude_xplane.h"
#include "compass_xplane.h"
#include "autopilot_xplane.h"
#include "nav_xplane.h"

// Main loop interval the period in uS: 30 ms
#define  MAIN_LOOP_INTERVAL     30000

#define AHRS_CONSTANTS_PATH "ahrs_constants"

ahrs           *TheAHRS                 = NULL;
airspeed       *TheAirspeed             = NULL;
altitude       *TheAltitude             = NULL;
compass        *TheCompass              = NULL;
gps            *TheGPS                  = NULL;
nav            *TheNAVNeedles           = NULL;
autopilot      *TheAutopilot            = NULL;

int main (int argc, char *argv[])
{
    unsigned            i;

    if (argc != 1)
    {
        fprintf (stderr, "Usage: test_avionics\n");
        return (-1);
    }
#if 0
    if (inet_aton (argv [1], &xplane_ip) == 0)
    {
        struct hostent  *host = gethostbyname (argv [1]);
        // Number a numbers and dots address. Must be a name
        if (host == NULL)
        {
            perror ("Invalid host");
            return (-1);
        }
        memcpy (&xplane_ip, host->h_addr_list[0], sizeof (xplane_ip));
    }
#endif

    try {
        // Instantiate and bind all avionics classes
        TheAHRS                     = new ahrs_cooked ();
        ahrs_xplane        *ax      = new ahrs_xplane ();
        TheAHRS->ConnectHardware (ax);
        SyntaxError *se = TheAHRS->InitConstants (AHRS_CONSTANTS_PATH);
        if (se != NULL)
        {
            fprintf (stderr, "Syntax error in %s, line %d: %s\n",
                    AHRS_CONSTANTS_PATH,
                    se->line, se->errorstring);
            delete se;
            delete TheAHRS;
            delete ax;
            return -1;
        }
        unsigned            ahrs_duty_cycle = TheAHRS->DutyCycle (MAIN_LOOP_INTERVAL);

        TheAirspeed                 = new airspeed();
        airspeed_xplane    *asx     = new airspeed_xplane();
        TheAirspeed->ConnectHardware (asx);
        unsigned            as_duty_cycle = TheAirspeed->DutyCycle (MAIN_LOOP_INTERVAL);

        TheAltitude                 = new altitude();
        altitude_xplane    *altx    = new altitude_xplane ();
        TheAltitude->ConnectHardware (altx);
        unsigned            alt_duty_cycle = TheAltitude->DutyCycle (MAIN_LOOP_INTERVAL);

        TheCompass                  = new compass();
        compass_xplane     *cpsx    = new compass_xplane ();
        TheCompass->ConnectHardware (cpsx);
        cpsx->ConnectAHRS (ax);
        unsigned            cps_duty_cycle = TheCompass->DutyCycle (MAIN_LOOP_INTERVAL);

        TheGPS                      = new gps();
        gps_xplane         *gx      = new gps_xplane ();
        TheGPS->ConnectHardware (gx);
        unsigned            g_duty_cycle = TheGPS->DutyCycle (MAIN_LOOP_INTERVAL);
        gx->ConnectAirspeed (asx);

        altx->ConnectGPS (gx);

        TheAutopilot                = new autopilot();
        autopilot_xplane   *apx     = new autopilot_xplane ();
        TheAutopilot->ConnectHardware (apx);
        unsigned            ap_duty_cycle = TheAutopilot->DutyCycle (MAIN_LOOP_INTERVAL);

        // Start main loop of sample and update
        unsigned            last_time, cur_time;

        last_time = time_in_us ();
        for (i = 0; 1; i++)
        {
            cur_time = time_in_us ();
            unsigned        dt = cur_time - last_time;
            if (dt < MAIN_LOOP_INTERVAL)
            {
                usleep (MAIN_LOOP_INTERVAL - dt);
            }
            last_time = cur_time;
            if (i % ahrs_duty_cycle == 0)
                TheAHRS->SampleAndCompute ();
            if (i % as_duty_cycle == 0)
                TheAirspeed->Update();
            if (i % alt_duty_cycle == 0)
                TheAltitude->Update();
            if (i % cps_duty_cycle == 0)
                TheCompass->Update();
            if (i % g_duty_cycle == 0)
                TheGPS->Update();
            if (i % ap_duty_cycle == 0)
                TheAutopilot->Update();
            if ((i == 100) && (TheAHRS->good))
            {
                TheAutopilot->SetAirspeedLimits (90, 290);
                TheAutopilot->EnableAutoCoordination ();
                TheAutopilot->SetAltitude (8300, 140, 270);
                TheAutopilot->SetHeading (46);
            }
            if ((i == 100) && (!TheAHRS->good))
                printf ("AHRS data not ready for autopilot.\n");
        }
    }
    catch (const int code)
    {
        fprintf (stderr, "Exception %d received!!\n", code);
        return (-2);
    }
    catch (...)
    {
        fprintf (stderr, "Unhandled Exception of unknown type received!!\n");
        return (-3);
    }
}
