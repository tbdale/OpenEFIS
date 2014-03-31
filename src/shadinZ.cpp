// shadinZ.cpp: Member functions of AHRS_XBOW class (attitude heading reference system)
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

#include "shadinZ.h"

using namespace std;

#define TRUE 1
#define FALSE 0


shadinZ::shadinZ(const char *unstrippedMsg)
{
  strncpy(rawMsg, unstrippedMsg, MAX_SHADIN_MSG_SIZE);
  rawMsg[MAX_SHADIN_MSG_SIZE] = '\0'; // Make sure string is terminated
  parseShadinZFormat();
}

// This method doesn't care if it's gps->adc or adc->gps data
// Since the Z field indentifiers don't overlap.
void shadinZ::parseShadinZFormat() {
  // First strip out the STX/ETX chars off of the raw message.
  int stxIdx = 0;
  int etxIdx = 0;
  for (int i=0; i<(int)sizeof(rawMsg); i++) {
    if (rawMsg[i] == 0x02) { // STX
      stxIdx = i;
    }
    else if (rawMsg[i] == 0x03) { // ETX
      etxIdx = i;
    }
  }

  for (int i=stxIdx; i <= etxIdx; i++) {
    printf("%c", rawMsg[i]);
  }
  printf("\n");

  if (stxIdx == etxIdx) {
    // TODO: make sure abort sig prob is fixed before enabling
    //ThrowException(string("SHADIN FORMAT EXCEPTION"));
  }

  // Create a C++ string to make the parsing easier.
  //string shadinZStr = string(rawMsg, etxIdx-stxIdx-1);  
  int checksum1BeginIdx = stxIdx;
  int checksum2BeginIdx = -1;
  for (int i=stxIdx+1; i<etxIdx; i++) {
    if (rawMsg[i] != 'Z') continue;
    char dataLabel = rawMsg[i+1]; // A, B, C, D, E, ...
    // From sniffing the protocol I believe all are caps
    char str[16];
    //bzero(str, 16);
    int len = 0;
    char tmpChars[16];
    int degTmp;
    double dblTmp;
    switch(dataLabel) {
    case 'A':
      len = 3;
      memcpy(str, &rawMsg[i+2], len);
      str[len] = '\0';
      iasKts = atoi(str);
      // checksum1BeginIdx = i; // Use the stxIdx instead
      i += 1+len+2; //  Skip over dataLabel + dataLen + <CR><NL>
      break;
    case 'B':
      len = 3;
      memcpy(str, &rawMsg[i+2], len);
      str[len] = '\0';
      tasKts = atoi(str);
      i += 1+len+2;
      break;
    case 'C':
      len = 3;
      memcpy(str, &rawMsg[i+2], len);
      str[len] = '\0';
      mach = atof(str)/1000.0;
      i += 1+len+2;
      break;
    case 'D':
      len = 5;
      memcpy(str, &rawMsg[i+2], len);
      str[len] = '\0';
      pAlt = atof(str) * 10.0;
      i += 1+len+2;
      break;
    case 'E':
      len = 5;
      memcpy(str, &rawMsg[i+2], len);
      str[len] = '\0';
      dAlt = atof(str) * 10.0;
      i += 1+len+2;
      break;
    case 'F':
      len = 3;
      memcpy(str, &rawMsg[i+2], len);
      str[len] = '\0';
      oatDegC = atof(str);
      i += 1+len+2;
      break;
    case 'G':
      len = 3;
      memcpy(str, &rawMsg[i+2], len);
      str[len] = '\0';
      tatDegC = atof(str);
      i += 1+len+2;
      break;
    case 'H':
      len = 3;
      memcpy(str, &rawMsg[i+2], len);
      str[len] = '\0';
      // If nothing there (i.e. dashes) then set to non-realistic value (-999999)
      if (strncmp(str,"---",3) == 0) windDir = -999999;
      else windDir = atoi(str);
      i += 1+len+2;
      break;
    case 'I':
      len = 3;
      memcpy(str, &rawMsg[i+2], len);
      str[len] = '\0';
      // If nothing there (i.e. dashes) then set to non-realistic value (-999999)
      if (strncmp(str,"---",3) == 0) windSpeedKts = -999999;
      else windSpeedKts = atoi(str);
      i += 1+len+2;
      break;
    case 'J':
      len = 3;
      memcpy(str, &rawMsg[i+2], len);
      str[len] = '\0';
      // If nothing there (i.e. dashes) then set to non-realistic value (-999999)
      if (strncmp(&str[1],"--",2) == 0) rateTurnDegSec = -999999.0;
      else rateTurnDegSec = atof(str); // + is right turn, left is -
      i += 1+len+2;
      break;
    case 'K':
      len = 4;
      memcpy(str, &rawMsg[i+2], len);
      str[len] = '\0';
      if (strncmp(&str[1],"---",3) == 0) vSpeedFpm = -999999;
      else vSpeedFpm = atoi(str) * 10; // Orig value is in tens of ft/min
      i += 1+len+2;
      break;
    case 'L':
      len = 3;
      memcpy(str, &rawMsg[i+2], len);
      str[len] = '\0';
      if (strncmp(str,"---",3) == 0) trueHeading = -999999;
      else trueHeading = atoi(str);
      i += 1+len+2;
      break;
    case 'M':
      len = 4;
      memcpy(str, &rawMsg[i+2], len);
      str[len] = '\0';
      rightFuelFlowGalHr = atof(str) / 10.0; // Orig is in tenths of gal/hr
      i += 1+len+2;
      break;
    case 'N':
      len = 5;
      memcpy(str, &rawMsg[i+2], len);
      str[len] = '\0';
      rightFuelUsedGal = atof(str) / 10.0; // Orig is in tenths of gal
      i += 1+len+2;
      break;
    case 'O':
      len = 4;
      memcpy(str, &rawMsg[i+2], len);
      str[len] = '\0';
      if (strncmp(str,"----",4) == 0) leftFuelFlowGalHr = -999999; // i.e If you get this then unit is just setup for one fuel flow sensor
      else leftFuelFlowGalHr = atof(str) / 10.0; // Orig is in tenths of gal/hr
      i += 1+len+2;
      break;
    case 'P':
      len = 5;
      memcpy(str, &rawMsg[i+2], len);
      str[len] = '\0';      
      if (strncmp(str,"-----",5) == 0) leftFuelUsedGal = -999999; // i.e If you get this then unit is just setup for one fuel flow sensor
      else leftFuelUsedGal = atof(str) / 10.0; // Orig is in tenths of gal
      i += 1+len+2;
      break;
    case 'Q':
      len = 3;
      memcpy(str, &rawMsg[i+2], len);
      str[len] = '\0';
      err = atoi(str); // Error code 001=temp_sensor_fail, 000=no_errors
      i += 1+len+2;
      break;
    case 'R':
      len = 5;
      memcpy(str, &rawMsg[i+2], len);
      str[len] = '\0';
      printf("Checksum extracted string: %s\n", str);
      checksum1 = atoi(str); // Error code 001=temp_sensor_fail, 000=no_errors
      //if ( checksumOk(rawMsg, stxIdx+1, i-1, checksum1) ) {
      if ( checksumOk(rawMsg, checksum1BeginIdx, i-1, checksum1) ) {
	// ThrowException("SHADIN CHECKSUM1 EXCEPTION"); TODO: enable this
      }
      i += 1+len+2;
      break;
    case 'S':
      len = 3;
      memcpy(str, &rawMsg[i+2], len);
      str[len] = '\0';
      groundSpeedKts = atoi(str);
      checksum2BeginIdx = i;
      i += 1+len+2;
      break;
    case 'T':
      len = 3;
      memcpy(str, &rawMsg[i+2], len);
      str[len] = '\0';
      magTrackDeg = atoi(str);
      i += 1+len+2;
      break;
    case 'U':
      len = 6;
      memcpy(str, &rawMsg[i+2], len);
      str[len] = '\0';
      distToDestNm = atoi(str);
      i += 1+len+2;
      break;
    case 'V':
      len = 4;
      memcpy(str, &rawMsg[i+2], len);
      str[len] = '\0';
      if (str[0] == 'E') {
	str[0] = '+'; // East is positive
      }
      else { // i.e. == 'W'
	str[0] = '-'; // West negative
      }
      magVarDeg = atoi(str);
      i += 1+len+2;
      break;
    case 'W':
      len = 7;
      memcpy(str, &rawMsg[i+2], len);
      str[len] = '\0';
      if (str[0] == 'N') {
	str[0] = '+'; // North is positive for latitude
      }
      else { // i.e. == 'S'
	str[0] = '-'; // South is negative
      }

      memcpy(tmpChars, str, 3);
      tmpChars[3] = '\0';
      degTmp = atoi(tmpChars);

      memcpy(tmpChars, &str[3], 2); // Get mm.mm  (decimal minutes)
      tmpChars[2] = '\0';
      dblTmp = atof(tmpChars);
      memcpy(tmpChars, &str[5], 2); // Get mm.mm  (decimal minutes)
      tmpChars[2] = '\0';
      dblTmp = dblTmp + atof(tmpChars)/100.0;
      latDeg = degTmp + dblTmp/60.0;

      i += 1+len+2;
      break;
    case 'X':
      len = 8;
      memcpy(str, &rawMsg[i+2], len);
      str[len] = '\0';
      if (str[0] == 'W') str[0] = '+'; // West is positive for longitude
      else str[0] = '-'; // East is negative  // i.e. == 'E'
      memcpy(tmpChars, str, 4);
      tmpChars[4] = '\0';
      degTmp = atoi(tmpChars);

      memcpy(tmpChars, &str[4], 2); // Get mm.mm  (decimal minutes)
      tmpChars[2] = '\0';
      dblTmp = atof(tmpChars);
      memcpy(tmpChars, &str[6], 2); // Get mm.mm  (decimal minutes)
      tmpChars[2] = '\0';
      dblTmp = dblTmp + atof(tmpChars)/100.0;
      lonDeg = degTmp + dblTmp/60.0;

      i += 1+len+2;
      break;
    case 'Y':
      len = 3;
      memcpy(str, &rawMsg[i+2], len);
      str[len] = '\0';
      checksum1 = atoi(str); // Error code 001=temp_sensor_fail, 000=no_errors
      if ( checksumOk(rawMsg, checksum2BeginIdx, i-1, checksum1) ) {
	// ThrowException("SHADIN CHECKSUM2 EXCEPTION"); TODO: enable this
      }
      i += 1+len+2;
      break;
    } // END of switch
  } // END of for
  
}


bool shadinZ::checksumOk(char *pktPtr, int dataIdxStart, 
                          int dataIdxStop, int checksum) {
  printf("start/stop: %d/%d\n", dataIdxStart, dataIdxStop);
  //unsigned short checksum = (pktPtr[checksumIdx] << 8) + pktPtr[checksumIdx+1];
  unsigned char sum_checked = 0;
  for (int i=dataIdxStart; i<=dataIdxStop; i++) {
    sum_checked += pktPtr[i];
  }
  //sum_checked = sum_checked;// % 0xFFFF;
  if (checksum != sum_checked) {
    printf("BAD SHADIN CHECKSUM ** Checksum/Sumchecked: %d/%d\n", checksum, sum_checked);
    return FALSE;
  }
  printf("GOOD SHADIN CHECKSUM -- Checksum/Sumchecked: %d/%d\n", checksum, sum_checked);
  return TRUE;
}


void shadinZ::printValues() {
  printf("iasKts:%d tasKts:%d mach:%f pAlt:%f dAlt:%f oatDegC:%f tatDegC:%f\n winDir:%d windSpeedKts:%d rateTurnDegSec:%f vSpeedFpm:%d trueHeading:%d\n rightFuelFlowGalHr:%f rightFuelUsedGal:%f leftFuelFlowGalHr:%f leftFuelUsedGal:%f err:%d checksum1:%d groundSpeedKts:%d magTrackDeg:%d distToDestNm:%d magVarDeg:%d latDeg:%f lonDeg:%f checksum2:%d\n",
	 iasKts, tasKts, mach, pAlt, dAlt, oatDegC, tatDegC, windDir,
         windSpeedKts, rateTurnDegSec, vSpeedFpm, trueHeading,
         rightFuelFlowGalHr, rightFuelUsedGal, leftFuelFlowGalHr, leftFuelUsedGal,
         err, checksum1, groundSpeedKts, magTrackDeg, distToDestNm, magVarDeg,
         latDeg, lonDeg, checksum2);
}
