// init_instruments.cpp: Instantiate instrument classes according to a configuration file.
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

#define AHRS_CONSTANTS_PATH "ahrs_constants"

ahrs           *TheAHRS                 = NULL;
airspeed       *TheAirspeed             = NULL;
altitude       *TheAltitude             = NULL;
compass        *TheCompass              = NULL;
gps            *TheGPS                  = NULL;
nav            *TheNAVNeedles           = NULL;
autopilot      *TheAutopilot            = NULL;

void InitInstruments (void)
{
    FILE        *conf;

    try {
        conf = fopen ("efis_config", "r");
        if (conf == NULL)
        {
            perror ("efis_config");
            ThrowException (NO_SUCH_FILE);
        }
        while (!feof (conf))
        {
            char        hardware_name [256];
            if (fscanf (conf, "%s", hardware_name) != 1)
            {
                if (feof (conf))
                    break;
                else
                {
                    perror ("efis_config");
                    ThrowException (FILE_ERROR);
                }
            }
            /* No Xplane stuff
            if (!strcmp (hardware_name, "xplane"))
            {
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
                    ThrowException (se);
                }
                unsigned            ahrs_duty_cycle = TheAHRS->DutyCycle (UPDATE_INTERVAL);
                TheAHRS->duty_cycle = ahrs_duty_cycle;

                TheAirspeed                 = new airspeed();
                airspeed_xplane    *asx     = new airspeed_xplane();
                TheAirspeed->ConnectHardware (asx);
                unsigned            as_duty_cycle = TheAirspeed->DutyCycle (UPDATE_INTERVAL);
                TheAirspeed->duty_cycle = as_duty_cycle;

                TheAltitude                 = new altitude();
                altitude_xplane    *altx    = new altitude_xplane ();
                TheAltitude->ConnectHardware (altx);
                unsigned            alt_duty_cycle = TheAltitude->DutyCycle (UPDATE_INTERVAL);
                TheAltitude->duty_cycle = alt_duty_cycle;

//                TheCompass                  = new compass();
//                compass_xplane     *cpsx    = new compass_xplane ();
//                TheCompass->ConnectHardware (cpsx);
//                cpsx->ConnectAHRS (ax);
//                unsigned            cps_duty_cycle = TheCompass->DutyCycle (UPDATE_INTERVAL);
//                TheCompass->duty_cycle = cps_duty_cycle;

                TheGPS                      = new gps();
                gps_xplane         *gx      = new gps_xplane ();
                TheGPS->ConnectHardware (gx);
                unsigned            g_duty_cycle = TheGPS->DutyCycle (UPDATE_INTERVAL);
                TheGPS->duty_cycle = g_duty_cycle;
                gx->ConnectAirspeed (asx);

                altx->ConnectGPS (gx);

                TheAutopilot                = new autopilot();
                autopilot_xplane   *apx     = new autopilot_xplane ();
                TheAutopilot->ConnectHardware (apx);
                unsigned            ap_duty_cycle = TheAutopilot->DutyCycle (UPDATE_INTERVAL);
                TheAutopilot->duty_cycle = ap_duty_cycle;
            } else {
                fprintf (stderr, "Unrecognized hardware type: %s\n", hardware_name);
                ThrowException (FILE_ERROR);
            }*/
        }
        fclose (conf);
    }

    catch (const int code)
    {
        fprintf (stderr, "Exception %d received!!\n", code);
        exit (-2);
    }
    catch (...)
    {
        fprintf (stderr, "Unhandled Exception of unknown type received!!\n");
        exit (-3);
    } 
}
