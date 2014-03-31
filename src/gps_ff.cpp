// gps_ff.cpp: Member functions of gps_ff class (gps FreeFlight sensor)
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

#include "constants.h"
#include "gps_ff.h"

#define NAV_PACKET_ID 0x51
#define NAV_PACKET_LEN 91
#define PFDE_RESPONSE_PACKET_ID 0x75
#define PFDE_RESPONSE_PACKET_LEN 35
#define GWSS_STATUS_PACKET_ID 0x5e
#define GWSS_STATUS_PACKET_LEN 10

 // TODO: autodetect architecture so don't assume little endian (intel, etc.)

gps_ff::gps_ff(const char *port)
{
    gps_hardware::gps_hardware();
    good = FALSE;
    serialPtr = new serial(port, TRUE);
    serialPtr->setHwFlowControl(0);
    serialPtr->setBaud(B19200);
    unsigned char startArray[1];
    startArray[0] = 0x10;
    unsigned char stopArray[2];
    stopArray[0] = 0x10;
    stopArray[1] = 0x03;
    serialPtr->setFrameStart(startArray, sizeof(startArray));
    serialPtr->setFrameEnd(stopArray, sizeof(stopArray));
    serialPtr->setMinMaxFrameLen(10, 91);
}

gps_ff::gps_ff()
{
    gps_ff::gps_ff("/dev/ttyUSB0");
}

void gps_ff::initialize() {
  //
}


/**
 * Sample
 * DESCRIPTION:     Sample gps data.
 * PRE-CONDITIONS:  Port is open to a valid gps device.
 * POST-CONDITIONS: good flag is FALSE, or all public gps data is correct.
 * EXCEPTIONS THROWN:  NO_IO_BOARD, BAD_IO_DRIVER ????TODO:
 * EXCEPTIONS HANDLED: None
 */
bool    // Returns TRUE if new data was available
        // or FALSE if the data remains unchanged as a result
        // of calling this function.
gps_ff::Sample()
{
  bool foundAtLeastOneDataFrame = FALSE;
  unsigned char framePtr[256];
  // Keep reading frames as longs as returning good ones...
  while(serialPtr->getFrame(framePtr) == TRUE) {

    unsigned char packetIdByte = framePtr[1]; // Packet id byte is always the second one.
    switch (packetIdByte) {
    case NAV_PACKET_ID:
      if (DEBUG) {
	printf("Found NAV_PACKET_ID\n");
	for (unsigned int a=0; a<NAV_PACKET_LEN; a++) {
	  printf("%02x ",framePtr[a]);
	}
	printf("\n");
      }
      foundAtLeastOneDataFrame = TRUE;
      parseNavPacket(framePtr);
      break;
    case PFDE_RESPONSE_PACKET_ID:
      if (DEBUG) printf("Found PFDE_RESPONSE_PACKET_ID\n");
      parsePfdeResponsePacket(framePtr);
      break;
    case GWSS_STATUS_PACKET_ID:
      if (DEBUG) {
	printf("Found GWSS_STATUS_PACKET_ID\n");
	for (unsigned int a=0; a<GWSS_STATUS_PACKET_LEN; a++) {
	  printf("%02x ",framePtr[a]);
	}
	printf("\n");
      }
      parseGwssStatusPacket(framePtr);
      break;
    default:
      printf("ERROR - Unrecognized packet id: %x\n", packetIdByte);
      break; // Some kind of corrupt frame - go on to the next DLE (start of frame) char
    }
  } // End of while() read frames loop

  return(foundAtLeastOneDataFrame);
}




bool gps_ff::parseNavPacket(unsigned char *pktPtr) {

  if (serial::checksumGood(pktPtr,0, 36, 37) == FALSE) return FALSE;
  // There's two checksums in this packet
  if (serial::checksumGood(pktPtr,0, 86, 87) == FALSE) return FALSE;

  unsigned char nav_state = pktPtr[2];
  float time_of_fix = parseFloatFromPacket(pktPtr, 5); // Secs since beg of UTC day that pos was computed
  lat = parseDoubleFromPacket(pktPtr, 9) * 180/M_PI;
  lng = parseDoubleFromPacket(pktPtr, 17) * 180/M_PI;
  gps_altitude = parseDoubleFromPacket(pktPtr, 25);
  float utc_time = parseFloatFromPacket(pktPtr, 33); // Secs since beg of UTC day
  

  if (DEBUG) printf("Lat=%f Lon=%f Alt=%f NavState=%d\n", lat, lng, gps_altitude, nav_state);
  if (DEBUG) printf("FixTimeSecs:UTCTimeSecs %f:%f\n", time_of_fix, utc_time);

  return TRUE;
}


bool gps_ff::parsePfdeResponsePacket(unsigned char *pktPtr) {
  if (serial::checksumGood(pktPtr, 0, 30, 31) == FALSE) return FALSE;
  return TRUE;
}

bool gps_ff::parseGwssStatusPacket(unsigned char *pktPtr) {
  if (serial::checksumGood(pktPtr, 0, 5, 6) == FALSE) return FALSE;


  return TRUE;
}


float gps_ff::parseFloatFromPacket(unsigned char *pktPtr, int startIdx) {
  float floatFoo = 0;
  memcpy(&floatFoo,  &pktPtr[startIdx], 4);
  serialPtr->byteswap((void *)&floatFoo, 4); // Change from big-endian to little
  return floatFoo;
}

double gps_ff::parseDoubleFromPacket(unsigned char *pktPtr, int startIdx) {
  double dblFoo = 0;
  memcpy(&dblFoo,  &pktPtr[startIdx], 8);
  serialPtr->byteswap((void *)&dblFoo, 8); // Change from big-endian to little
  return dblFoo;
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
gps_ff::TimeBase (void) const
{
  return 0;
}
