// fad_fdatasystems.cpp: Member functions of Flight Data Systems air data computer class
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
#include "fad_fdatasystems.h"
#include "shadinZ.h"

using namespace std;

fad_fdatasystems::fad_fdatasystems(const char *port)
{
    good = FALSE;
    serialPtr = new serial(port, TRUE);
    initialize();
}


void fad_fdatasystems::initialize() {
    serialPtr->setHwFlowControl(0);
    serialPtr->setBaud(B9600);
    unsigned char startArray[1];
    startArray[0] = 0x02; // STX
    //startArray[1] = 0x5A; // Z
    unsigned char stopArray[1];
    stopArray[0] = 0x03; // ETX
    serialPtr->setFrameStart(startArray, sizeof(startArray));
    serialPtr->setFrameEnd(stopArray, sizeof(stopArray));
    serialPtr->setFixedFrameLen(139); // TODO: is this globally correct for fad->gps data?? (or just works with Fdatasystems unit)
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
fad_fdatasystems::Sample()
{
  bool foundAtLeastOneDataFrame = FALSE;
  unsigned char framePtr[256];
  // Keep reading frames as longs as returning good ones...
  try {
    while(serialPtr->getFrame(framePtr) == TRUE) {
      /*for (int i=0; i<256; i++) {
	printf("%02x: ", framePtr[i]);
	}*/
      parseDataFrame(framePtr);
    } 
  }
  catch(string errStr) {
    // TODO: 
    return FALSE;
  }

  return(foundAtLeastOneDataFrame);
}

void fad_fdatasystems::parseDataFrame(unsigned char *framePtr) {
  shadinZ *shadinZPtr = NULL;
  try {
    shadinZPtr = new shadinZ((char *)framePtr);
    // TODO: set member variables
    shadinZPtr->printValues(); // TODO: remove debug later
  }
  catch(string errStr) {
    if (shadinZPtr != NULL) delete(shadinZPtr); // Cleanup
    ThrowException(errStr); // Rethrow
  }
}


