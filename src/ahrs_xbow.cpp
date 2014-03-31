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
#include "ahrs_xbow.h"

#define ANGLE_CONV_FACTOR float(180.0/32768);
#define RADIAN_CONV_FACTOR float(M_PI/32768);
#define RADIAN_RATE_CONV_FACTOR float((M_PI/180) * (1200.0/32768));
#define ACCEL_G_CONV_FACTOR float(15.0/32768);
#define ACCEL_MS_CONV_FACTOR float(LOCAL_GRAVITY*15.0/32768);

using namespace std;

ahrs_xbow::ahrs_xbow (const char *port)
{
    ahrs_hardware::ahrs_hardware();

    good = FALSE;
    serialPtr = new serial(port, TRUE);
    initialize();
    dt = XBOW_FRAME_PERIOD;
 }

ahrs_xbow::ahrs_xbow ()
{
    ahrs_xbow::ahrs_xbow ("/dev/ttyUSB0");
}

void ahrs_xbow::initialize() {
   serialPtr->setHwFlowControl(0);
   serialPtr->setBaud(B38400);
   unsigned char startArray[2];
   startArray[0] = 0xAA;
   startArray[1] = 0x55;
   /*
   unsigned char stopArray[2];
   stopArray[0] = 0x10;
   stopArray[1] = 0x03;
   */
   serialPtr->setFrameStart(startArray, sizeof(startArray));
   //serialPtr->setFrameEnd(stopArray, sizeof(stopArray));
   serialPtr->setFixedFrameLen(26); // Uses fixed length frame of only one type
   
   serialPtr->writePort("a", 1); // Setting AHRS mode (send "a")
   // Initialize to continuous data streaming (send "C")
   serialPtr->writePort("C", 1);
}


/**
 * Sample
 * DESCRIPTION:     Sample xbow data.
 * PRE-CONDITIONS:  Port is open to a valid xbow device.
 * POST-CONDITIONS: good flag is FALSE, or all public angle data is correct.
 * EXCEPTIONS THROWN:  NO_IO_BOARD, BAD_IO_DRIVER ????TODO:
 * EXCEPTIONS HANDLED: None
 */
bool    // Returns TRUE if new data was available
                // or FALSE if the data remains unchanged as a result
                // of calling this function.
ahrs_xbow::Sample (void)
{
  unsigned char framePtr[256];
  // Keep reading frames as longs as returning good ones...
  try {
    while(serialPtr->getFrame(framePtr) == TRUE) {
      continue;
    }
  }
  catch(string errStr) {
    fprintf(stderr, errStr.c_str());
    serialPtr->initializePort(); // Always try to put port back in good state if any probs
    return FALSE;
  }

  // Now parse the most recent data frame you just found.
  if ( parseDataFrame(framePtr) == TRUE ) {
    return TRUE;
  }
  return FALSE;
}





bool ahrs_xbow::parseDataFrame(unsigned char *framePtr) {
  if (serial::checksumGood(framePtr,2,23,24) == FALSE) return FALSE;
  // TODO: set member variables
  // TODO: check conversion because python scripts differ by a degree sometimes (rounding???)
  roll_cooked =    (((short)(framePtr[2]<<8)  + (short)framePtr[3])) * RADIAN_CONV_FACTOR; // radians
  pitch_cooked =   (((short)(framePtr[4]<<8)  + (short)framePtr[5])) * RADIAN_CONV_FACTOR;
  heading_cooked = (((short)(framePtr[6]<<8)  + (short)framePtr[7])) * RADIAN_CONV_FACTOR;
  //short hdg_raw = ((short)(framePtr[6]<<8)  + (short)framePtr[7]);
  //printf("Heading raw: %d\n", hdg_raw);
  if (heading_cooked <= 0) heading_cooked += 2*M_PI; // Change the scale from -PI->PI to 0->2PI
  ang_roll =     (((short)(framePtr[8]<<8)  + (short)framePtr[9]))  * RADIAN_RATE_CONV_FACTOR; // radians/sec
  ang_pitch =    (((short)(framePtr[10]<<8) + (short)framePtr[11])) * RADIAN_RATE_CONV_FACTOR;
  ang_head =      (((short)(framePtr[12]<<8) + (short)framePtr[13])) * RADIAN_RATE_CONV_FACTOR;
  accel_yaw =  (((short)(framePtr[14]<<8) + (short)framePtr[15])) * ACCEL_MS_CONV_FACTOR; // g's
  accel_lift =  (((short)(framePtr[16]<<8) + (short)framePtr[17])) * ACCEL_MS_CONV_FACTOR; // g's
  accel_thrust =  (((short)(framePtr[18]<<8) + (short)framePtr[19])) * ACCEL_MS_CONV_FACTOR; // g's
  int model_num =     ((short)(framePtr[20]<<8)  + (short)framePtr[21]); 
  int status_short =     ((short)(framePtr[22]<<8)  + (short)framePtr[23]); 
  
  // Status bits
  int hard_failure      = status_short & 0x0001;
  int soft_failure      = status_short & 0x0002;
  int not_ready         = status_short & 0x0004;
  int power_fail        = status_short & 0x0008;
  int comm_error        = status_short & 0x0010;
  int reboot_detect     = status_short & 0x0020;
  int bad_caltable      = status_short & 0x0040;
  int turn_indicator    = status_short & 0x0080;

  if (hard_failure || soft_failure || not_ready || power_fail || comm_error || reboot_detect || bad_caltable) {
    good = FALSE;
  } else {
    good = TRUE;
  }
  /*int algo_status       = status_short && 0x0200; // TODO: Finish magnetic calibration stuff
  int magcal_inprogress = status_short && 0x0400;
  int magcal_badstoreddata    = status_short && 0x0800;
  int magcal_unsatisfactory   = status_short && 0x1000;
  */



  printf("roll/pitch/heading angle %f/%f/%f\n", roll_cooked, pitch_cooked, heading_cooked);
  /*printf("model_num: %d\n", model_num);
  printf("status flags %d:%d:%d:%d:%d:%d:%d:%d\n", 
         hard_failure, soft_failure, not_ready, power_fail, 
         comm_error, reboot_detect, bad_caltable, turn_indicator);
  */
  return TRUE;
}



