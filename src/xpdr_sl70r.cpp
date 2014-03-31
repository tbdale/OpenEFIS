// xpdr_sl70r.cpp: Member functions of Apollo SL70R remote transponder
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
#include "xpdr_sl70r.h"

#define ALT_MSG_LEN 17
#define STATUS_MSG_LEN 11
#define MODE_MSG_LEN 17
#define REPLYCNT_MSG_LEN 11
#define CONF_MSG_LEN 16
#define ALTHOLD_MSG_LEN 17

using namespace std;

xpdr_sl70r::xpdr_sl70r(const char *port)
{
    serialPtr = new serial(port, TRUE);
    initialize();
}



void xpdr_sl70r::initialize() {
    good = FALSE;
    altFt = 0;
    degC = 15;
    modeCharSend = 'O'; // "O"=standby, "A"=ModeA; "C"=ModeC
    identCharSend = '-'; // "-"=inactive; "I"=enabled
    squawkCodeSend = 1200;
    status2Bytes = 0xFFFF; // Assume good status until otherwise recvd
    serialPtr->setHwFlowControl(0);
    serialPtr->setBaud(B9600);
    setModeStandby();
}

// Returns TRUE if new data was available
// or FALSE if the data remains unchanged as a result
// of calling this function.
bool xpdr_sl70r::Sample() {
  // Look for all msg types
  int msgLen[] = {ALT_MSG_LEN, STATUS_MSG_LEN, MODE_MSG_LEN, REPLYCNT_MSG_LEN,CONF_MSG_LEN,ALTHOLD_MSG_LEN};

  int idx[6];
  while(1) {
    idx[0] = serialPtr->peekBufForStartCharsPlusFrameLength((unsigned char *)"#AL", 3, ALT_MSG_LEN);
    idx[1] = serialPtr->peekBufForStartCharsPlusFrameLength((unsigned char *)"^SS", 3, STATUS_MSG_LEN);
    idx[2] = serialPtr->peekBufForStartCharsPlusFrameLength((unsigned char *)"^MD", 3, MODE_MSG_LEN);
    idx[3] = serialPtr->peekBufForStartCharsPlusFrameLength((unsigned char *)"^RC", 3, REPLYCNT_MSG_LEN);
    idx[4] = serialPtr->peekBufForStartCharsPlusFrameLength((unsigned char *)"^C1", 3, CONF_MSG_LEN);
    idx[5] = serialPtr->peekBufForStartCharsPlusFrameLength((unsigned char *)"^AH", 3, ALTHOLD_MSG_LEN);
    
    int tmpIdx = 1000000;
    int frameLen = -1;
    for (int i=0; i < 6; i++) {
      if (idx[i] == -1) continue;
      if (idx[i] < tmpIdx) {
	tmpIdx = idx[i];
	frameLen= msgLen[i];
      }
    }
    
    unsigned char msg[32]; // 32 chars will take care of longest message.
    memset(msg, '\0', 32); // terminate str
    if (tmpIdx == 1000000) return FALSE; // No more messages found
    serialPtr->getFrameFromBufAndShift(msg, tmpIdx, frameLen);
    parseMessage((char *)msg);
  }

  return TRUE;
}


void xpdr_sl70r::parseMessage(char *msg) {
  if (strstr(msg, "#AL") != NULL) {
    if (DEBUG) printf("RECEIVED ALT MSG: %17s\n", msg);
    if (!checksumOk(msg, 0, 13, 14)) return;
    handleAltMsg(msg);
  }
  else if (strstr(msg, "^SS") != NULL) {
    if (DEBUG) printf("RECEIVED STATUS MSG: %11s\n", msg);
    if (!checksumOk(msg, 0, 8, 9)) return;
    handleStatusMsg(msg);
  }
  else if (strstr(msg, "^MD") != NULL) {
  if (DEBUG) printf("RECEIVED MODE MSG: %17s\n", msg);
    if (!checksumOk(msg, 0, 13, 14)) return;
    handleModeMsg(msg);
  }
  else if (strstr(msg, "^RC") != NULL) {
    if (DEBUG) printf("RECEIVED REPLY CNT MSG: %11s\n", msg);
    if (!checksumOk(msg, 0, 7, 8)) return;
    handleReplyCountMsg(msg);
  }
  else if (strstr(msg, "^C1") != NULL) {
    if (DEBUG) printf("RECEIVED SW VER MSG: %16s\n", msg);
    if (!checksumOk(msg, 0, 8, 9)) return;
    handleSoftwareVersionMsg(msg);
  }
  else if (strstr(msg, "^AH") != NULL) {
    if (DEBUG) printf("RECEIVED ALT HOLD MSG: %16s\n", msg);
    if (!checksumOk(msg, 0, 13, 14)) return;
    handleAltHoldMsg(msg);
  }
}

void xpdr_sl70r::handleAltMsg(char *msg) {
  sscanf(&msg[4], "%d", &squawkAltFlLvl);
  // Check for error conditions
  if (squawkAltFlLvl == -9981) {
    if (DEBUG) printf("ERROR: POSSIBLE HARDWARE PROB\n");
    errorHardwareBad = TRUE;
  }
  else if (squawkAltFlLvl == -9982) {
    if (DEBUG) printf("ERROR: SQUAWK_ALT_FL OUT OF RANGE\n");
    errorAltOutOfRange = TRUE;
  }
  else { // All good...
    if (DEBUG) printf("SQUAWK_ALT_FL=: %d\n", squawkAltFlLvl);
    errorAltOutOfRange = FALSE;
    errorHardwareBad = FALSE;
  }
}


void xpdr_sl70r::handleStatusMsg(char *msg) {
  sscanf(&msg[4], "%4x", &status2Bytes);
  if (DEBUG) printf("RECEIVED STATUS BYTES: %X\n", status2Bytes);
  // TODO: add status flags for each but condition
  if ((status2Bytes & 0x0001) == 0) {
    // 3.3 volt power supply prob
  }
  if ((status2Bytes & 0x0002) == 0) {
    // 5 volt power supply prob
  }
  if ((status2Bytes & 0x0004) == 0) {
    // 8 volt power supply prob
  }
  if ((status2Bytes & 0x0008) == 0) {
    // 12 volt power supply prob
  }
  if ((status2Bytes & 0x0010) == 0) {
    // 45 volt power supply prob
  }
  if ((status2Bytes & 0x0020) == 0) {
    // high voltage power supply prob
  }
  if ((status2Bytes & 0x0040) == 0) {
    // input voltage prob
  }
  if ((status2Bytes & 0x0080) == 0) {
    // discrete input reference V prob
  }
  if ((status2Bytes & 0x0100) == 0) {
    // display temperature prob
  }
  if ((status2Bytes & 0x0200) == 0) {
    // transmitter temperature prob
  }
  if ((status2Bytes & 0x0300) == 0) {
    // photo cell prob
  }
  if ((status2Bytes & 0x0400) == 0) {
    // synthesizer locked prob
  }
  if ((status2Bytes & 0x0800) == 0) {
    // receiver test prob
  }
  if ((status2Bytes & 0x1000) == 0) {
    // transmitter test prob
  }
  if ((status2Bytes & 0x2000) == 0) {
    // supression stuck prob
  }
}


void xpdr_sl70r::handleModeMsg(char *msg) {
  // Set member variables
  sscanf(&msg[4], "%c,%c,%4i", &modeCharReceive, &identCharReceive, &squawkCodeReceive);
  if (DEBUG) printf("MODE_CHAR=%c  IDENT_CHAR=%c  SQUAWK_RECV=%04d\n", modeCharReceive, identCharReceive, squawkCodeReceive);
}


void xpdr_sl70r::handleReplyCountMsg(char *msg) {
  // Set replyCnt member variable
  sscanf(&msg[4], "%4i", &replyCount);
  if (DEBUG) printf("REPLY_CNT=%d\n", replyCount);
}

void xpdr_sl70r::handleSoftwareVersionMsg(char *msg) {
  //sprintf(softwareVersion, "Micro SW version %3c, FPGA SW version %2c MODE %c Transponder", 
  //	  &msg[4],&msg[8],&msg[10]);
  sprintf(softwareVersion, "Micro SW ver %3.3s FPGA SW ver %2.2s Mode %1.1s Transponder",
	  &msg[4], &msg[8], &msg[10]);
  if (DEBUG) printf("RECEIVED SW VER MSG: %s\n", softwareVersion);
}


void xpdr_sl70r::handleAltHoldMsg(char *msg) {
  if (DEBUG) printf("RECEIVED ALT HOLD MSG: %17s\n", msg);
  // TODO: figure out format for this msg type (not listed in manual?)
  // "^AH -+00000,03DE" is a captured msg example - what do these indicate?
}

void xpdr_sl70r::sendMsgToUnit(char *msg, int numChars) {
  // TODO: where to catch exception from write???
  char msgTmp[64];
  unsigned short checksum = calculateChecksum(msg, 0, numChars-1);
  char hexTmp[4];
  sprintf(hexTmp, "%02X", checksum);
  memcpy(msgTmp, msg, numChars);
  memcpy(&msgTmp[numChars], hexTmp, 2);
  msgTmp[numChars+2] = '\r'; // All msgs are terminated with CR
  int numWrote = serialPtr->writePort(msg, numChars+3);
  if (DEBUG) printf("sendMsgToUnit string=%s bytes_wrote=%d\n", msgTmp, numWrote);
}

void xpdr_sl70r::resetUnit() {
  sendMsgToUnit("#RS", 3);
  // "#" = msgstart; "RS" = reset_msg; "C8" = checksumHex; "\r"= msg termination
}

void xpdr_sl70r::setAltitude(int altFeet, int degCels) {
  altFt = altFeet;
  degC = degCels;
  char msgTmp[32];
  sprintf(msgTmp, "#AL %+.5dT%+.2d", altFt, degC);
  // #=msgstart; AL=altitude_msg; 5d alt; letter T; 2d temp_cels
  sendMsgToUnit(msgTmp, 14);
}


bool xpdr_sl70r::checksumOk(char *msg, 
			    int msgStartIdx, 
			    int msgStopIdx, 
			    int checksumIdx) {
  char tmp[32];
  sprintf(tmp, "0X%c%c", msg[checksumIdx], msg[checksumIdx+1]);
  unsigned short sentChecksum = 0;
  sscanf(tmp, "%x", &sentChecksum);
 
  unsigned short calcChecksum = calculateChecksum(msg, msgStartIdx, msgStopIdx);
  if (calcChecksum != sentChecksum) {
    if (DEBUG) printf("BAD CHECKSUM sent:calc  %2X:%2X\n", sentChecksum, calcChecksum);
    return FALSE;
  }
  return TRUE;
}


// TODO: Need to refactor and move to serial class?
// I didn't use the serial class' checksum method because
// here they use checksums that aer stored in ascii-hex
unsigned short xpdr_sl70r::calculateChecksum(char *msgPtr, 
					     int msgStartIdx,
					     int msgStopIdx) {
  unsigned short checksum = 0;
  for (int i=msgStartIdx; i<=msgStopIdx; i++) {
    checksum += msgPtr[i];
    //if (DEBUG) printf("<<<%d:%d:%d:%02X>>>\n",i,msgPtr[i], checksum, checksum);
  }
  checksum = checksum & 0xFF; // i.e only a one-byte checksum
  if (DEBUG) printf("FINAL<<<%d:%02X>>>\n",checksum, checksum);
  return checksum;
}


void xpdr_sl70r::setModeA() {
  modeCharSend = 'A';
  setMode();
}

void xpdr_sl70r::setModeC() {
  modeCharSend = 'C';
  setMode();
}

void xpdr_sl70r::setModeStandby() {
  modeCharSend = 'O';
  setMode();
}

void xpdr_sl70r::setMode() {
  char msgTmp[32];
  sprintf(msgTmp, "#MD %c,%c,%04d", modeCharSend, identCharSend, squawkCodeSend);
  sendMsgToUnit(msgTmp, 12);
}

void xpdr_sl70r::enableIdent() {
  identCharSend = 'I';
  setMode();
}

void xpdr_sl70r::disableIdent() {
  identCharSend = '-';
  setMode();
}


void xpdr_sl70r::setSquawk(int code) {
  // Check for out of bounds codes and set to VFR if bad
  if (code < 0 || code > 7000) squawkCodeSend = 1200;
  else if ((code%1000) >  700) squawkCodeSend = 1200;
  else if ((code%100)  >   70) squawkCodeSend = 1200;
  else if ((code%10)   >    7) squawkCodeSend = 1200;
  else   squawkCodeSend = code;

  setMode();
}

void xpdr_sl70r::setSquawk(char *squawk) {
  setSquawk(atoi(squawk));
}


