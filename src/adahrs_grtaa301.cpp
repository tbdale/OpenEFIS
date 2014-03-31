// ahrs_xbow.cpp: Member functions of AHRS_XBOW class (attitude heading reference system)
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
#include <string>

#include "constants.h"
#include "adahrs_grtaa301.h"

//#define ANGLE_CONV_FACTOR float(180.0/32768);
//#define RADIAN_CONV_FACTOR float(M_PI/32768);
//#define RADIAN_RATE_CONV_FACTOR float((M_PI/180) * (1200.0/32768));
//#define ACCEL_G_CONV_FACTOR float(15.0/32768);
//#define ACCEL_MS_CONV_FACTOR float(LOCAL_GRAVITY*15.0/32768);
#define NUM_OF_MSG_TYPES  4
#define HIGH_RATE_PRIMARY_DATA_MSG_LEN  23
#define LOW_RATE_PRIMARY_DATA_MSG_LEN   21
#define USER_CALIBRATION_MSG_LEN        25
#define MAINTENANCE_MSG_LEN             36

#define HIGH_RATE_PRIMARY_DATA_MSG_HDR "\x7F\xFF\xFE"
#define LOW_RATE_PRIMARY_DATA_MSG_HDR  "\x7F\xFF\xFD"
#define USER_CALIBRATION_MSG_HDR       "\x7F\xFF\xFC"
#define MAINTENANCE_MSG_HDR            "\x7F\xFF\xFB"


// NOTE/TODO: Didn't include some msg types that weren't used
// in normal operation (i.e. calibration, ROM load, etc.)

using namespace std;

adahrs_grtaa301::adahrs_grtaa301 (const char *port)
{
  // TODO: fix // ahrs_hardware::ahrs_hardware();
  //good = FALSE;
    serialPtr = new serial(port, TRUE);
    initialize();
    //dt = GRT_FRAME_PERIOD;
 }


void adahrs_grtaa301::initialize() {
   serialPtr->setHwFlowControl(0);
   serialPtr->setBaud(B19200);
}


// Returns TRUE if new data was available
// or FALSE if the data remains unchanged as a result
// of calling this function.
bool adahrs_grtaa301::Sample() {
  // Look for all msg types
  int msgLen[] = {HIGH_RATE_PRIMARY_DATA_MSG_LEN,
		  LOW_RATE_PRIMARY_DATA_MSG_LEN,
		  USER_CALIBRATION_MSG_LEN,
		  MAINTENANCE_MSG_LEN};

  int idx[4];
  while(1) {
    idx[0] = serialPtr->peekBufForStartCharsPlusFrameLength((unsigned char *)HIGH_RATE_PRIMARY_DATA_MSG_HDR, 3, HIGH_RATE_PRIMARY_DATA_MSG_LEN);
    idx[1] = serialPtr->peekBufForStartCharsPlusFrameLength((unsigned char *)LOW_RATE_PRIMARY_DATA_MSG_HDR, 3, LOW_RATE_PRIMARY_DATA_MSG_LEN);
    idx[2] = serialPtr->peekBufForStartCharsPlusFrameLength((unsigned char *)USER_CALIBRATION_MSG_HDR, 3, USER_CALIBRATION_MSG_LEN);
    idx[3] = serialPtr->peekBufForStartCharsPlusFrameLength((unsigned char *) MAINTENANCE_MSG_HDR, 3, MAINTENANCE_MSG_LEN);

    int tmpIdx = 1000000;
    int frameLen = -1;
    for (int i=0; i < NUM_OF_MSG_TYPES; i++) {
      if (idx[i] == -1) continue;
      if (DEBUG) printf("Found possible frame i=%d\n", i);
      if (idx[i] < tmpIdx) {
        tmpIdx = idx[i];
        frameLen= msgLen[i];
      }
    }

    unsigned char msg[36]; // 32 chars will take care of longest message.
    memset(msg, '\0', 36); // terminate str
    if (tmpIdx == 1000000) return FALSE; // No more messages found
    serialPtr->getFrameFromBufAndShift(msg, tmpIdx, frameLen);
    parseMessage(msg);
  }

  return TRUE;
}



bool adahrs_grtaa301::parseMessage(unsigned char *msg) {
  if (memcmp(msg,HIGH_RATE_PRIMARY_DATA_MSG_HDR,3) == 0) {
    if (DEBUG) printf("RECEIVED HIGH_RATE_PRIMARY_DATA_MSG: \n");
    if (!checksumOk(msg, 2, 21, 22)) return FALSE;
    handleHighRatePrimaryDataMsg(msg);
    return TRUE;
  }
  else if (memcmp(msg,LOW_RATE_PRIMARY_DATA_MSG_HDR,3) == 0) {
    if (DEBUG) printf("RECEIVED  LOW_RATE_PRIMARY_DATA_MSG: \n");
    if (!checksumOk(msg, 2, 19, 20)) return FALSE;
    handleLowRatePrimaryDataMsg(msg);
    return TRUE;
  }
  else if (memcmp(msg, USER_CALIBRATION_MSG_HDR,3) == 0) {
  if (DEBUG) printf("RECEIVED  USER_CALIBRATION_MSG: \n");
    if (!checksumOk(msg, 2, 23, 24)) return FALSE;
    handleUserCalibrationMsg(msg);
    return TRUE;
  }
  else if (memcmp(msg,MAINTENANCE_MSG_HDR,3) == 0) {
    if (DEBUG) printf("RECEIVED MAINTENANCE_MSG: \n");
    if (!checksumOk(msg, 2, 38, 39)) return FALSE;
    handleMaintenanceMsg(msg);
    return TRUE;
  }
  return FALSE;
}


bool adahrs_grtaa301::checksumOk(unsigned char *checkMsg, 
				  int startIdx, 
				  int stopIdx, 
				  int checksumIdx) {
  unsigned short sum = 0;
  
  for (int i=startIdx; i<=stopIdx; i++) {
    sum += checkMsg[i];
  }
  //if (DEBUG) printf("Calculated raw checksum (no 1's complement yet): %d\n", sum);
  // NOTE: Grand Rapids doesn't care about overflow/carries
  // Use carries to compute 1's complement sum.
  /*
  sum = (sum >> 8) + (sum & 0x00FF);
  sum += sum >> 8; // One final carry is possible
  */

  // Now invert the 8-bit result.
  unsigned char calculatedChecksum = ((unsigned char) ~sum);

  // Get the checksum included in the packet
  unsigned char extractedChecksum = (unsigned char)checkMsg[checksumIdx];

  if (calculatedChecksum != extractedChecksum) {
    if (DEBUG) printf("CHECKSUM ERROR calculated:extracted  %d:%d\n", calculatedChecksum, extractedChecksum);
    return FALSE;
  }
  return TRUE;
}



void adahrs_grtaa301::handleHighRatePrimaryDataMsg(unsigned char *msg) {
  unsigned char status_byte = msg[3]; // = 0 for normal
  printf("Status byte: %X\n", status_byte);
  // TODO: make these use the inherited members where available
  float roll_cooked =    (((short)(msg[4]<<8)  + (short)msg[5])) * M_PI/32768; // +/- PI radians: positive is right wing down
  //printf("roll_raw = %d\n",   ((short)(msg[4]<<8)  + (short)msg[5]));
  float pitch_cooked =   (((short)(msg[6]<<8)  + (short)msg[7])) * M_PI/32768; // +/- PI radians: positive nose up
  int heading_cooked = (((short)msg[8]<<8) + (short)msg[9]) * 180/32768;
  if (heading_cooked <= 0) heading_cooked += 360; // Change the scale from -180->180 to 0->360
  int pressure_alt = (unsigned short)(msg[10]<<8)  + (unsigned short)msg[11] - 5000; // Convert for 5K offset
  int vert_speed_fpm = (short)(msg[12]<<8)  + (short)msg[13];
  float ias_kts = ((short)(msg[14]<<8)  + (short)msg[15]) * 0.00987473002; // fpm-->kts (1 feet per minute = 0.00987473002 knots)
  // NOTE: don't really need right now //int ias_rate_fpm = ((short)(msg[16]<<8)  + (short)msg[17]); // Airspeed acceleration
  int slipball_deg = ((short)(msg[18]<<8)  + (short)msg[19]); // +/-180 positive values right wind down
  int z_g = ((short)(msg[20]<<8)  + (short)msg[21]); // Positive for aircraft accel upward, data is in sensor (aircraft) frame
  // TODO: check scaling on z_g (look in spec to see if need normalize by 32k?)


  printf("roll:pitch:heading    %6.4f:%6.4f:%d\n", roll_cooked, pitch_cooked, heading_cooked);
  printf("pAlt:vertSpd:ias:incl:g    %d:%d:%4.1f:%d:%d\n", pressure_alt, vert_speed_fpm, ias_kts, slipball_deg, z_g);

}

void adahrs_grtaa301::handleLowRatePrimaryDataMsg(unsigned char *msg) {
  strcat((char *)msg,"FOO"); // get rid of warning for now
  // TODO:
}

void adahrs_grtaa301::handleUserCalibrationMsg(unsigned char *msg) {
  strcat((char *)msg,"FOO"); // get rid of warning for now
  // TODO:
}

void adahrs_grtaa301::handleMaintenanceMsg(unsigned char *msg) {
  strcat((char *)msg,"FOO"); // get rid of warning for now
  // TODO:
}
