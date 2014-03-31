// comm_sl40.cpp: Interfaces with Apollo SL40 transceiver unit via serial port
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

#include "exceptions.h"
#include "constants.h"
#include "comm_sl40.h"

#define TRANSCEIVER_STATUS_MSG_ID "01"
#define SOFTWARE_VERSION_MSG_ID   "03"
#define SET_ACTIVE_FREQ_MSG_ID    "00"
#define SET_ACTIVE_FREQ_MSG_LEN    14
#define SET_STANDBY_FREQ_MSG_ID    "01"
#define SET_STANDBY_FREQ_MSG_LEN    14

#define SL40_NMEA0183_HEADER "$PMRRC" // http://pcptpp030.psychologie.uni-regensburg.de/trafficresearch/NMEA0183/


using namespace std;

comm_sl40::comm_sl40(const char *port)
{
    good = FALSE;
    serialPtr = new serial(port, TRUE);
    initialize();
}




void comm_sl40::initialize() {
    serialPtr->setHwFlowControl(0);
    serialPtr->setBaud(B9600);
    standbyFrequencyKhz = 121500; // ie. 121.5 - Always a good default
    standbyFrequencyKhz = 122000; // ie. 122.0 - Flightwatch is good to start with
    monitorStandbyFlag = FALSE;

    // Set the framing for the data read from the comm unit
    unsigned char startArray[1];
    startArray[0] = '$'; // Dollar sign NMEA 0183 format supposedly
    unsigned char stopArray[1];
    stopArray[0] = '\r'; // ETX
    serialPtr->setFrameStart(startArray, sizeof(startArray));
    serialPtr->setFrameEnd(stopArray, sizeof(stopArray));
    serialPtr->setMinMaxFrameLen(11, 20); // Should take care of all msgs
}


/**
 * Sample
 * DESCRIPTION:     Samples status data output by transveiver.
 * PRE-CONDITIONS:  Port is open to a valid comm_sl40 device.
 * POST-CONDITIONS: good flag is FALSE, or all public gps data is correct.
 * EXCEPTIONS THROWN:  NO_IO_BOARD, BAD_IO_DRIVER ????TODO:
 * EXCEPTIONS HANDLED: None
 */
bool    // Returns TRUE if new data was available
        // or FALSE if the data remains unchanged as a result
        // of calling this function.
        // Status messages are output once/sec so this method
        // should be called with that frequency or greater.
comm_sl40::Sample()
{
  bool foundAtLeastOneDataFrame = FALSE;
  unsigned char framePtr[256];
  // Keep reading frames as longs as returning good ones...
  try {
    while(serialPtr->getFrame(framePtr) == TRUE) {
      parseMessage((char *)framePtr);
    } 
  }
  catch(string errStr) {
    // TODO: 
    return FALSE;
  }

  return(foundAtLeastOneDataFrame);
}

void comm_sl40::parseMessage(char *framePtr) {
  // First get msg id - 7th and 8th frame chars
  if (strncmp(&framePtr[6], TRANSCEIVER_STATUS_MSG_ID, 2) == 0) {
    if (!checksumOk(framePtr, 6, 13, 14)) return;
    // Pull out the status info
    unsigned short actMhzTmp = framePtr[8] + 0x30;
    unsigned short actKhzTmp = (framePtr[9] - 0x30) * 25;
    activeFrequencyKhz = (actMhzTmp*1000) + actKhzTmp; // Set member variable

    unsigned short sbyMhzTmp = framePtr[10] + 0x30;
    unsigned short sbyKhzTmp = (framePtr[11] - 0x30) * 25;
    standbyFrequencyKhz = (sbyMhzTmp*1000) + sbyKhzTmp; // Set member variable

    transceiverStatus = framePtr[12]; // Set member variable
    squelchTestSetting = framePtr[13]; // Set member variable
    return;
  }
  if (strncmp(&framePtr[6], SOFTWARE_VERSION_MSG_ID, 2) == 0) {
    if (!checksumOk(framePtr, 6, 11, 12)) return;
    // Pull out the s/w version info
    sprintf(softwareVersion, "%4s", framePtr[8]); // Set member variable
    return;
  }
  // TODO: If here throw exception for unknown msg
}


void comm_sl40::calculateChecksum(char *ptr, int startIdx, int stopIdx, char *checksum2Char) {
  unsigned short checksum = 0;
  for (int i=startIdx; i <= stopIdx; i++) {
    //checksum = checksum ^ ptr[i]; // It says to ignore carry (by bit or byte?)
    checksum = checksum + ptr[i]; // It says to ignore carry (by bit or byte?)
  }
  unsigned char checkbyte = checksum && 0x00FF; // mask off first byte of two

  checksum2Char[0] = ((checkbyte && 0xF0)>>4) + 0x30; // Take most sig nibble, shift and add hex 30
  checksum2Char[1] = (checkbyte && 0x0F) + 0x30; // Take least sig nibble and add hex 30
}


bool  comm_sl40::checksumOk(char *msg, int startIdx, int stopIdx, int checksumIdx) {
  char checksum[2];
  calculateChecksum(msg, startIdx, stopIdx, checksum);

  if (strncmp(&msg[checksumIdx], checksum, 2) != 0) {
    if (DEBUG) printf("BAD CHECKSUM calc:extracted  %2s:%2s\n", checksum, &msg[checksumIdx]);
    return FALSE;
  }
  return TRUE;
}

int comm_sl40::getActiveFrequencyKhz() {
  return this->activeFrequencyKhz;
}

void comm_sl40::setActiveFrequencyKhz(int kHz) {
  char msgTmp1[32];
  char m = kHz/1000 - 0x30; // integer part
  char k = (kHz%1000/25) + 0x30; // decimal part
  
  char transceiverFunction; // transceiverFunction: N=normal_recv, M=enable_monitor, 0=no_change
  if (monitorStandbyFlag == TRUE) {
    transceiverFunction = 'M';
  }
  else {
    transceiverFunction = 'N';
  }

  sprintf(msgTmp1, "%6s%2s%c%c%c", SET_ACTIVE_FREQ_MSG_ID, 
	  SL40_NMEA0183_HEADER, m, k, transceiverFunction); 
  char checksum[2];
  calculateChecksum(msgTmp1, 6, 10, checksum);
  char msgTmp2[32];
  sprintf(msgTmp2, "%s%2s\r", msgTmp1, checksum);
  serialPtr->writePort(msgTmp2, SET_ACTIVE_FREQ_MSG_LEN);
}



int comm_sl40::getStandbyFrequencyKhz() {
  return this->standbyFrequencyKhz;
}

void comm_sl40::setStandbyFrequencyKhz(int kHz) {
  char msgTmp1[32];
  char m = kHz/1000 - 0x30; // integer part
  char k = (kHz%1000/25) + 0x30; // decimal part
  
  char transceiverFunction; // transceiverFunction: N=normal_recv, M=enable_monitor, 0=no_change
  if (monitorStandbyFlag == TRUE) {
    transceiverFunction = 'M';
  }
  else {
    transceiverFunction = 'N';
  }

  sprintf(msgTmp1, "%6s%2s%c%c%c", SET_STANDBY_FREQ_MSG_ID, 
	  SL40_NMEA0183_HEADER, m, k, transceiverFunction); 
  char checksum[2];
  calculateChecksum(msgTmp1, 6, 10, checksum);
  char msgTmp2[32];
  sprintf(msgTmp2, "%s%2s\r", msgTmp1, checksum);
  serialPtr->writePort(msgTmp2, SET_STANDBY_FREQ_MSG_LEN);
}


void comm_sl40::setMonitorStandby(bool monitorFlag) {
  monitorStandbyFlag = monitorFlag;
  // Now that the flag is set do a getStandby/setStandby
  // to send this info to the unit without changing anything else.
  // (basically a NOOP cmd to unit)
  setStandbyFrequencyKhz(getStandbyFrequencyKhz());
}

void comm_sl40::setNewFrequencyListTag(char* listTag, char listNum) {
  // TODO: ??? (or leave this functionality in the efis because
  //            other comm unit vendors may not support this)
}

void comm_sl40::setNewFrequencyListItem(char listNum, char freqType, int freqKhz) {
  // TODO: ??? (or leave this functionality in the efis because
  //            other comm unit vendors may not support this)
}

char comm_sl40::getTransceiverStatus() {
  return this->transceiverStatus;
}

char comm_sl40::getSquelchStatus() {
  return this->squelchTestSetting;
}
