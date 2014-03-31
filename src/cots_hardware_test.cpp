// cots_hardware_test.cpp: A crude tester for the COTS hardware devices
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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#include "ahrs_xbow.h"
#include "fad_fdatasystems.h"
#include "gps.h"
#include "compass.h"
#include "gps_ff.h"
#include "xpdr_sl70r.h"
#include "adahrs_grtaa301.h"
#include "comm_sl40.h"


gps            *TheGPS                  = NULL;
compass        *TheCompass              = NULL;

int main (int argc, char *argv[])
{
  if (argc != 4) {
    fprintf(stderr, "Please enter <classname> <port> <poll_sleep_interval_microsecs> on cmd line\n");
    fprintf(stderr,
	    "  (Classnames supported: ahrs_xbow, fad_fdatasystems, gps_ff, xpdr_sl70r, adahrs_grtaa301, comm_sl40.h)\n");
    exit(-1);
  }

  char *classname = argv[1];
  char *port      = argv[2];
  long sleepusecs = atoi(argv[3]);

  fad_fdatasystems *fd;
  adahrs_grtaa301 *adahrs;
  xpdr_sl70r *xpdr;
  gps_ff *gps;
  ahrs_xbow *ahrs;
  comm_sl40 *comm;

  if (strcmp(classname, "fad_fdatasystems") == 0) {
    fd = new fad_fdatasystems(port);
  }
  else if (strcmp(classname, "adahrs_grtaa301") == 0) {
    adahrs = new adahrs_grtaa301(port);

    while (1) {
      //printf("Calling sample\n");
      bool newDataFlag = adahrs->Sample();
      if (!newDataFlag) {
	printf("No new data\n");
      }
      else {
	printf("FOUND New data\n");
      }
      usleep(sleepusecs); // Simulate poll
    }
  }
  else if (strcmp(classname, "xpdr_sl70r") == 0) {
    xpdr = new xpdr_sl70r(port);
  }
  else if (strcmp(classname, "gps_ff") == 0) {
    gps = new gps_ff(port);
  }
  else if (strcmp(classname, "ahrs_xbow") == 0) {
    ahrs = new ahrs_xbow(port);
  }
  else if (strcmp(classname, "comm_sl40") == 0) {
    comm = new comm_sl40(port);
  }

}

