// gps_xplane.cpp: GPS data reader for X-Plane simulation
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
#include "exceptions.h"

#include "airspeed_xplane.h"
#include "gps_xplane.h"

extern char    rcv_buffer [1500];

/**
 * Sample
 * DESCRIPTION:     Sample sensor data.
 * PRE-CONDITIONS:  io_board_fd is open to a valid device driver.
 * POST-CONDITIONS: good flag is FALSE, or all public angle data is correct.
 * EXCEPTIONS THROWN:  NO_IO_BOARD, BAD_IO_DRIVER
 * EXCEPTIONS HANDLED: None
 */
bool    // Returns TRUE if new data was available
                // or FALSE if the data remains unchanged as a result
                // of calling this function.
gps_xplane::Sample ()
{
    struct sockaddr_in          lp;
    int                         size, fromlen;
    int                         type;
    bool                        ret;

    lp.sin_family = AF_INET;
    lp.sin_port = 49000;
    lp.sin_addr.s_addr = INADDR_LOOPBACK;

    fromlen = sizeof (lp);

    assert (sizeof(int) == sizeof(float));
    ret = FALSE;
    while (1)
    {
        size = recvfrom (io_board_fd, rcv_buffer, sizeof (rcv_buffer), 0,
                         (struct sockaddr*) &lp, (socklen_t*)&fromlen);
        if (size == -1)
            break;

        memcpy (&type, rcv_buffer, sizeof(type));
        switch (type)
        {
            case XPT_LAT_LONG_ALT:
                //printf ("LAT, LONG, ALT:\n"
                //        "  %15f%15f%15f%15f\n"
                //        "  %15f%15f%15f%15f\n",
                //        ((float*)rcv_buffer) [1],
                //        ((float*)rcv_buffer) [2],
                //        ((float*)rcv_buffer) [3],
                //        ((float*)rcv_buffer) [4],
                //        ((float*)rcv_buffer) [5],
                //        ((float*)rcv_buffer) [6],
                //        ((float*)rcv_buffer) [7],
                //        ((float*)rcv_buffer) [8]);
                if (++oversample_counter >= XP_FRAME_RATE)
                {
                    ret = TRUE;
                    oversample_counter = 0;
                    tm = time(0);
                    lat = ((float*) rcv_buffer) [1];
                    lng = ((float*) rcv_buffer) [2];
                    track = 0;
                    if (as != NULL)
                        speed = (unsigned) roundf (as->vgrnd);
                }
                altitude = ((float*) rcv_buffer) [3];
                good = TRUE;
                break;
            case XPT_GPS:
                //printf ("GPS:\n"
                //        "  %15f%15f%15f%15f\n"
                //        "  %15f%15f%15f%15f\n",
                //        ((float*)rcv_buffer) [1],
                //        ((float*)rcv_buffer) [2],
                //        ((float*)rcv_buffer) [3],
                //        ((float*)rcv_buffer) [4],
                //        ((float*)rcv_buffer) [5],
                //        ((float*)rcv_buffer) [6],
                //        ((float*)rcv_buffer) [7],
                //        ((float*)rcv_buffer) [8]);
                break;
            default:
                ThrowException (INVALID_MSG);
                break;
        }
    }
    return (ret);
}


/**
 * TimeBase
 * DESCRIPTION:     Returns how many times per second a new history entry
 *                  arrives. Returns 0 if a sample is taken every time 'Sample'
 *                  is called.
 * PRE-CONDITIONS:  None
 * POST-CONDITIONS: None
 * EXCEPTIONS THROWN:  None
 * EXCEPTIONS HANDLED: None
 */
float   // See Description
    gps_xplane::TimeBase (void) const
{
    return (1.0);
}

gps_xplane::gps_xplane ()
{
    struct sockaddr_in  lcl;

    ::gps_hardware();

    altitude = 0;
    good = FALSE;
    as = NULL;

    oversample_counter = 0;

    io_board_fd = socket (PF_INET, SOCK_DGRAM, 0);
    if (io_board_fd == -1)
    {
        perror ("gps_xplane: socket");
        ThrowException(errno);
    }
    lcl.sin_family = AF_INET;
    lcl.sin_port = htons(48000);
    lcl.sin_addr.s_addr = INADDR_ANY;

    bind (io_board_fd, (struct sockaddr*) &lcl, sizeof (lcl));
    fcntl (io_board_fd, F_SETFL, O_NONBLOCK);
}
