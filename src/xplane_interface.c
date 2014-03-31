// xplane_interface.c: The interface to the X-Plane flight simulator
//                     for testing.
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

// Takes UDP data packets in from X-Plane. Splits them up into their repsective categories,
// and then sends them to various destination sockets.
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <assert.h>

#include "xplane_interface.h"

char    rcv_buffer [1500];
char    xmt_buffer[1500];

int main (int argc, char *argv[])
{
    int                 fd, apfd, maxfd;
    struct sockaddr_in  lcl, xpl, gps, ahrs, airspeed, nav, autopilot;
    struct in_addr      xplane_ip;

    if (argc != 2)
    {
        fprintf (stderr, "Usage: xplane_interface <xplane_host_address>\n");
        return (-1);
    }
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
    fd = socket (PF_INET, SOCK_DGRAM, 0);
    apfd = socket (PF_INET, SOCK_DGRAM, 0);
    if ((fd == -1) || (apfd == -1))
    {
        perror ("socket");
        return (-1);
    }
    if (fd > apfd)
        maxfd = fd;
    else
        maxfd = apfd;

    lcl.sin_family = AF_INET;
    lcl.sin_port = htons (49000);
    lcl.sin_addr.s_addr = htonl (INADDR_ANY);

    xpl.sin_family = AF_INET;
    xpl.sin_port = htons (49000);
    memcpy (&xpl.sin_addr, &xplane_ip, sizeof (xplane_ip));

    gps.sin_family = AF_INET;
    gps.sin_port = htons (48000);
    gps.sin_addr.s_addr = htonl (INADDR_LOOPBACK);

    ahrs.sin_family = AF_INET;
    ahrs.sin_port = htons (48001);
    ahrs.sin_addr.s_addr = htonl (INADDR_LOOPBACK);

    airspeed.sin_family = AF_INET;
    airspeed.sin_port = htons (48004);
    airspeed.sin_addr.s_addr = htonl (INADDR_LOOPBACK);

    nav.sin_family = AF_INET;
    nav.sin_port = htons (48005);
    nav.sin_addr.s_addr = htonl (INADDR_LOOPBACK);

    autopilot.sin_family = AF_INET;
    autopilot.sin_port = htons (48006);
    autopilot.sin_addr.s_addr = htonl (INADDR_LOOPBACK);

    if (bind (fd, (struct sockaddr*) &lcl, sizeof (lcl)) == -1)
    {
        perror ("bind");
        return (-1);
    }
    if (connect (fd, (struct sockaddr*) &xpl, sizeof (xpl)) == -1)
    {
        perror ("connect");
        return (-1);
    }
    if (bind (apfd, (struct sockaddr*) &autopilot, sizeof (autopilot)) == -1)
    {
        perror ("bind");
        return (-1);
    }
    while (1)
    {
        int     size, offset, fromlen;
        fd_set  rfds;
        struct timeval  tv;

        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        FD_SET(apfd, &rfds);
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        if (select (maxfd+1, &rfds, NULL, NULL, &tv) <= 0)
            continue;
        if (FD_ISSET (fd, &rfds))
        {
            fromlen = sizeof (xpl);
            size = recvfrom (fd, rcv_buffer, sizeof (rcv_buffer), 0,
                             (struct sockaddr*) &xpl, &fromlen);
            if (size == -1)
                break;
            assert (!strncmp (rcv_buffer, "DATA", 4));
            for (offset = 6; offset + SIZEOF_XPT_DATA <= size; offset += SIZEOF_XPT_DATA)
            {
                int type;
                struct sockaddr_in *to;
                to = NULL;
                memcpy (&type, rcv_buffer + offset, sizeof(type));
                switch (type)
                {
                    case XPT_SPEED_VSI:
                        to = &airspeed;
                        break;
                    case XPT_ENG_THRUST:
                    case XPT_AERO_FORCE:
                    case XPT_ANG_VEL:
                    case XPT_PRH:
                        to = &ahrs;
                        break;
                    case XPT_LAT_LONG_ALT:
                    case XPT_GPS:
                    case XPT_XYZ:
                        to = &gps;
                        break;
                    case XPT_NAV_DEFLECT:
                        to = &nav;
                    default:
                        break;
                }
                if (to != NULL)
                {
                    memcpy (xmt_buffer, rcv_buffer + offset, SIZEOF_XPT_DATA);
                    sendto (fd, xmt_buffer, SIZEOF_XPT_DATA, 0,
                            (struct sockaddr*)to, sizeof (ahrs));
                }
            }
        }
        if (FD_ISSET (apfd, &rfds))
        {
            struct sockaddr_in  ap;
            ap.sin_family = AF_INET;
            ap.sin_port = 0;
            ap.sin_addr.s_addr = htonl (INADDR_LOOPBACK);
            size = recvfrom (apfd, rcv_buffer, sizeof (rcv_buffer), 0,
                             (struct sockaddr*) &ap, &fromlen);
            if (size <= 0)
                break;
            send (fd, rcv_buffer, size, 0);
        }
    }
    close(fd);
    close(apfd);
    return (0);
}
