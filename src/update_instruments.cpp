
// update_instruments.cpp: Test the avionics package using an interface to X-Plane
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

#include <qgl.h>

#include "efis.h"
#include "pfd.h"
#include "hsi.h"

void EFIS::UpdateInstruments ()
{
     static unsigned     i = 0;
     bool                update_pfd, update_hsi;
//
     update_pfd = FALSE;
     update_hsi = FALSE;
     int degrees;
//     try {
//         if (i % TheAHRS->duty_cycle == 0)
//         {
//             TheAHRS->SampleAndCompute ();
//             if (PFD && TheAHRS->good)
//             {
//                 PFD->setRoll ((TheAHRS->roll_angle * 180 / M_PI));
//                 PFD->setPitch (-((TheAHRS->pitch_angle * 180 / M_PI)));
//                 update_pfd = TRUE;
//             }

                  if (PFD)
                  {
                      update_pfd = TRUE;
                  }
                  if (HSI)
                  { 
                      update_hsi=TRUE;
                  }

//         }
//         if (i % TheAirspeed->duty_cycle == 0)
//         {
//             TheAirspeed->Update();
//             if (PFD && TheAirspeed->good)
//             {
//                 PFD->setIAS (TheAirspeed->as);
//                 update_pfd = TRUE;
//             }
//         }
//         if (i % TheAltitude->duty_cycle == 0)
//         {
//             TheAltitude->Update();
//             if (PFD && TheAltitude->good)
//             {
//                 PFD->setMSL (TheAltitude->alt);
//                 PFD->setVSI ((int)TheAltitude->alt_prime);
//                 update_pfd = TRUE;
//             }
//         }
//         if (i % TheCompass->duty_cycle == 0)
//         {
//             TheCompass->Update();
//             if (HSI && TheCompass->good)
//             {
//                 HSI->setMagHeading(TheCompass->heading);
//                 update_hsi = TRUE;
//             }
//         }
//         if (i % TheGPS->duty_cycle == 0)
//         {
//             TheGPS->Update();
//         }
//         if (i % TheAutopilot->duty_cycle == 0)
//             TheAutopilot->Update();
//
          if (update_pfd)
              PFD->updateGL();
          if (update_hsi)
              HSI->updateGL();
//          i++;
//      }
//     catch (const int code)
//     {
//         fprintf (stderr, "Exception %d received!!\n", code);
//     }
//     catch (...)
//     {
//         fprintf (stderr, "Unhandled Exception of unknown type received!!\n");
//     }
}
