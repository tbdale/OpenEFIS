#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <stdlib.h>
#include <errno.h>
#include <string>


#include "serial.h"
#include "exceptions.h"
#include "constants.h"

#define _POSIX_SOURCE 1


#define DEFAULT_CFLAG B9600 | CRTSCTS | CS8 | CLOCAL | CREAD;
#define DEFAULT_OPEN_SYNC_FLAGS O_RDWR | O_NOCTTY;
#define DEFAULT_OPEN_ASYNC_FLAGS O_RDWR | O_NOCTTY | O_NONBLOCK;
#define DEFAULT_IFLAG IGNPAR;
#define DEFAULT_OFLAG 0;
#define DEFAULT_LFLAG 0;  /* set input mode (non-canonical, no echo,...) */
#define DEFAULT_CHAR_READ_TIMEOUT_TENTHSECS 0;   /* inter-character timer unused */
#define DEFAULT_NUM_CHARS_BLOCK_READ 0;   /* block or signal read until/when N chars received */
#define SER_READ_BUF_SIZE 2048; /* TODO: will data frames ever be larger than this? */
#define MAX_NUM_TO_READ_PER_SAMPLE 2048

using namespace std;

serial::serial(const char* portPtr, int asyncFlg) {
    numRead  = -1;
    numWrote = -1;
    //readCharBuf =;
    //portName = (char *)malloc(80);
    strcpy(portName, portPtr);
    asyncFlag = asyncFlg;
    initializePort();
}

serial::serial(const char* portPtr) {
  serial(portPtr, TRUE); // Default to async
}

serial::~serial() {
  closePort();
}


void serial::initializePort() {
  if (fd>=0) {
    closePort(); // Just in case it was open previously
  }

  if (asyncFlag == TRUE) {
    portOpenFlags = DEFAULT_OPEN_ASYNC_FLAGS;
    if (DEBUG) printf("Set to async mode\n");
  }
  else {
    portOpenFlags = DEFAULT_OPEN_SYNC_FLAGS;
  }

  openPort(); 
  if (DEBUG) printf("INITIALIZED PORT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n"); // TODO: remove dbug
}



void serial::openPort() {
    struct termios newtio;
    fd = open(portName, portOpenFlags); 
    if (fd <0) {
      perror("OPEN ERROR: "); // TODO: remove this debug later??
      ThrowException( string(strerror(errno)) ); //return;
    }

    if (asyncFlag == TRUE) {
      /* Make the file descriptor asynchronous (the manual page says only 
	 O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
      fcntl(fd, F_SETFL, FASYNC);
    }

    memset(&newtio, '\0', sizeof(newtio));
    newtio.c_cflag = DEFAULT_CFLAG;
    newtio.c_iflag = DEFAULT_IFLAG;
    newtio.c_oflag = DEFAULT_OFLAG;
    newtio.c_lflag = DEFAULT_LFLAG;
    
    newtio.c_cc[VTIME]    = DEFAULT_CHAR_READ_TIMEOUT_TENTHSECS;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = DEFAULT_NUM_CHARS_BLOCK_READ;   /* blocking read until 1 chars received */
    
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&newtio);
}

void serial::closePort(void) {
  close(fd);
}

void serial::setBaud(int baud) {
    struct termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, baud);
    cfsetospeed(&options, baud);
    options.c_cflag |= (CLOCAL | CREAD);
    tcsetattr(fd, TCSANOW, &options);
}

void serial::setHwFlowControl(int on) {
    // ON on=1, OFF on=0
    struct termios options;
    tcgetattr(this->fd, &options);
    if (on) {
        options.c_cflag |= CRTSCTS;
    }
    else {
        options.c_cflag &= ~CRTSCTS;
    }
    tcsetattr(this->fd, TCSANOW, &options);
}

void serial::setReadTimeout(int tenthsOfSecs) {
    struct termios options;
    tcgetattr(this->fd, &options);
    options.c_cc[VTIME] = tenthsOfSecs;  /* inter-character timer */
    tcsetattr(this->fd, TCSANOW, &options);
}


int serial::readPort(int numChars) {
    //memset(readCharBuf, '\0', sizeof(readCharBuf));
    numRead = read(this->fd, this->readCharBuf, numChars);

    if (numRead <0) {
      ThrowException(strerror(errno)); //return;
    }

    return numRead;
}

int serial::writePort(char *buf, int numChars) {
    numWrote = write(this->fd, buf, numChars);
    if (numWrote <0) {
      perror("WRITE ERROR:"); // TODO: remove debug
      //initializePort(); // Reinitialize port (cable reconnect etc.)
      ThrowException(strerror(errno)); //return;
    }
    return numWrote;
}

void serial::setFrameStart(unsigned char* startChars, int numChars) {
  startFrameCharCnt = numChars;
  startFrameChars = (unsigned char*)malloc(numChars);
  memcpy(this->startFrameChars, startChars, numChars);

}

void serial::setFrameEnd(unsigned char* stopChars, int numChars) {
  stopFrameCharCnt = numChars;
  stopFrameChars = (unsigned char*)malloc(numChars);
  memcpy(this->stopFrameChars, stopChars, numChars);
}

void serial::readAvailableData() {
  // TODO: Is it possible for the read to spend too much time here or get stuck??
  while (readPort(MAX_NUM_TO_READ_PER_SAMPLE) > 0) { // Keep grabbing data until no bytes
    if (DEBUG) printf("\nreadPort/numRead=%d\n", numRead);
    if (numInFrameBuf + numRead > (int)sizeof(frameDataBuf)) {
      // Wrap buffer if about to exceed it!
      // TODO: To be most correct take partial frame data with you when wrapping.
      numInFrameBuf = 0;
    }
    memcpy(&frameDataBuf[numInFrameBuf], readCharBuf, numRead); // Append to frame Buffer
    numInFrameBuf += numRead;
  }
}



int serial::getStartCharsBufIdx(unsigned char *startChars, 
				int startCharCnt, 
				unsigned char *stopChars, 
				int stopCharCnt) {
  bool found = FALSE;
  int i;
  for (i=0; i<numInFrameBuf-startCharCnt; i++) { 
    // For above you subtract startFrameCharCnt because of "peeking" ahead in code below
    int matchedCharCnt = 0;
    for (int j=0; j<startCharCnt; j++) {
      if (frameDataBuf[i+j] == startChars[j]) {
	//if (DEBUG) printf("\nframeDataBuf: %02x\n", frameDataBuf[i+j]);
	// If one/more of start framing chars match then look
        // at the next char ahead in the buf to make sure it's
        // not escaped. (e.g. if start char is DLE and you see
        // two DLEs together then this is not a start frame - but
        // COMPRESS the chars together when you do find complete frame.)
	if (frameDataBuf[i+j] == frameDataBuf[i+j+1]) {
          i++; // Skip over this char back at top of loop
          break;
	}
	matchedCharCnt++;
	//if (DEBUG) printf("Matched hdr char_cnt/idx/hex_char: %d/%d/%x\n", matchedCharCnt, i+j, frameDataBuf[i+j]);
      }
      else {
	break; // Prevents looking at next start frame char if first char didn't match
      }
    }

    // Make sure end of frame doesn't fool it if end-of-frame marker looks
    // like start of frame. (e.g. DLE.....DLE ETX)
    if (stopChars != NULL && stopCharCnt > 0) { // Sanity check to make sure stop chars exist/needed.
      if (startCharCnt == 1 && 
	  stopCharCnt > 1   && 
	  frameDataBuf[i+1] == stopChars[1] ) { // Just check 2nd stop char for now TODO:??
	continue; // Go around you found a stop frame not a start...
      }
    }

     if (matchedCharCnt < startCharCnt) continue; // Keep lookin - no frame start found yet.
 
    found = TRUE;
    break;
  }

  if (found) {
    if (DEBUG) printf("FOUND START OF PACKET/FRAME (i/numInFrameBuf=%d/%d)\n", i, numInFrameBuf);
    return i; 
  }
  return -1;
}


bool serial::getFrame(unsigned char* framePtr) {
  readAvailableData(); // Read all available data in member buf variable
  int i;
  while((i=getStartCharsBufIdx(startFrameChars, startFrameCharCnt, stopFrameChars, stopFrameCharCnt)) != -1) {
    if (DEBUG) { 
      if (DEBUG) printf("2 (i=%d)>>>>> ",i);
      for (unsigned int a=0; a<256; a++) {
	if (DEBUG) printf("%02x ",frameDataBuf[a]);
      }
      if (DEBUG) printf("\n");
    }

    int numLeftInBufMinusStartChars = numInFrameBuf - i;

    // FIRST: Handle fixed-length frame case
    // Now if you are using a fixed frame length then see if you can grab enough for fixed frame and return
    // TODO: consider start char escaping (e.g. DLE/DLE pairs)
    // Can we assume fixed length frames have 2 char start headers so that no char escaping
    // is needed for this case?
    if (fixedFrameLen != 0 ) {
      if (numLeftInBufMinusStartChars+startFrameCharCnt < fixedFrameLen) return FALSE;
      getFrameFromBufAndShift(framePtr, i, fixedFrameLen);
      /*
      if (DEBUG) printf("FFL>>>>> ");
      for (unsigned int a=0; a<256; a++) {
	if (DEBUG) printf("%02x ",frameDataBuf[a]);
      }
      if (DEBUG) printf("\n");
      */
      if (DEBUG) printf("FOUND FIXED LENGTH FRAME (i/numInFrameBuf=%d/%d)\n", i, numInFrameBuf);
      return TRUE;
    }

    // NEXT: Handle variable-length frame case
    // (If you are here it is a variable length frame)

    if (minFrameLen != 0 && 
        (numLeftInBufMinusStartChars+startFrameCharCnt) < minFrameLen) {
      // Sanity check - Get outa here if not enough bytes for minimum frame
      return FALSE;
    }
    
    bool foundNextFrameStart = FALSE;
    int k;
    for (k=i+1; k<numInFrameBuf-startFrameCharCnt; k++) {
      // Look for next start of header char(s) to make sure you
      // have one full frame before escaped char reduction, etc.
      int matchedCharCnt = 0;
      for (int m=0; m<startFrameCharCnt; m++) {
	if (frameDataBuf[k+m] == startFrameChars[m]) matchedCharCnt++;
      }
      if (matchedCharCnt == startFrameCharCnt &&
          (k-i) >= minFrameLen ) {
	// Also check that the frame end doesn't look like the start (e.g. DLE....DLE ETX)
        if (stopFrameCharCnt > 1 && 
	    startFrameChars[0] == stopFrameChars[0] &&
            frameDataBuf[k+1] == stopFrameChars[1] ) {
	  continue;
	}
	foundNextFrameStart = TRUE;
	break;
      }
    }
    if (foundNextFrameStart == FALSE) return FALSE;
    // If you are here - you have found a complete frame
    int uncompressedFrameLen = k-i;
    if (DEBUG) printf("FOUND END(next start) OF VARIABLE LEN FRAME (idx %d)\n", k);

    if (DEBUG) {
      printf("3 (i=%d>>>>> ", i);
      for (unsigned int a=0; a<256; a++) {
	printf("%d/%02x ", a, frameDataBuf[a]);
      }
      printf("\n");
    }

    getFrameFromBufAndShift(framePtr, i, uncompressedFrameLen);    

    if (DEBUG) {
      printf("PFB>>>>> ");
      for (unsigned int a=0; a<256; a++) {
	//printf("%02X ",framePtr[a]);
	printf("%c",framePtr[a]);
      }
      printf("\n");
    }

    // If needed, compress double startFrame char that is not part of framing (i.e. data byte)
    if (startFrameCharCnt == 1) {
      int tmpFrameLen = uncompressedFrameLen;
      for (int n=startFrameCharCnt-1; n<uncompressedFrameLen-1; n++) {
	if (framePtr[n] == startFrameChars[0] &&
	    framePtr[n] == framePtr[n+1] ) {
	  memmove(&framePtr[n], &framePtr[n+1], tmpFrameLen - (n+1));
	  tmpFrameLen--; // Set new frame len
	}
      }
    }

    return TRUE;
  } // End of while()
  
  return(FALSE);
}



void serial::getFrameFromBufAndShift(unsigned char *pktBuf, 
                                      int bufIdx, 
                                      int packetLen) {
  if (DEBUG) {
    printf("PFD idx=%d  packetLen=%d\n", bufIdx, packetLen);
    printf("BUF_B4_SHIFT=");
    for (int i=0; i<numInFrameBuf; i++) printf("%X:%d ", frameDataBuf[i], frameDataBuf[i]);
    printf("\n");
  }
  memcpy(pktBuf, &frameDataBuf[bufIdx], packetLen);
  if (numInFrameBuf == bufIdx+packetLen) {
    numInFrameBuf = 0; // i.e. The packet border ended at the end of buffer - no byte shifting needed.
  }
  else {
    memmove(frameDataBuf, &frameDataBuf[bufIdx+packetLen], numInFrameBuf-bufIdx+packetLen);
    numInFrameBuf -= packetLen;
  }
}


void serial::setMinMaxFrameLen(int shortest, int longest) {
  minFrameLen = shortest;
  maxFrameLen = longest;
}

void serial::setFixedFrameLen(int len) {
  fixedFrameLen = len;
  minFrameLen = len;
  maxFrameLen = len;
}

// Note: This method assumes a two-byte checksum
bool serial::checksumGood(unsigned char *pktPtr, int dataIdxStart, int dataIdxStop, int checksumIdx) {
  //int numBytes = sizeof(pktPtr);
  //printf("MSB:%x LSB: %x\n", pktPtr[24], pktPtr[25]);
  unsigned short checksum = (pktPtr[checksumIdx] << 8) + pktPtr[checksumIdx+1];
  unsigned short sum_checked = 0;
  for (int i=dataIdxStart; i<=dataIdxStop; i++) {
    sum_checked += pktPtr[i];
  }
  //sum_checked = sum_checked;// % 0xFFFF;
  if (checksum != sum_checked) {
    printf("BAD CHECKSUM ** Checksum/Sumchecked: %d/%d\n", checksum, sum_checked);
    return FALSE;
  }
  //printf("GOOD CHECKSUM -- Checksum/Sumchecked: %d/%d\n", checksum, sum_checked);
  return TRUE;
}



void serial::byteswap(void *ptr, int len)
{
  unsigned char b[len],c[len];
  (void) memcpy(b,ptr,len); 
  for (int i=0; i<len; i++) {
    c[i]=b[len-1-i]; /* swap data around */
  }
  (void) memcpy(ptr,c,len);
}


// This function looks for frames that have start-chars, no end-chars, and
// a fixed length. 
int serial::peekBufForStartCharsPlusFrameLength( unsigned char *peekChars, 
						 int numPeekChars, 
						 int minCharsNeeded) {
  readAvailableData(); // Read all available data in member buf variable
  int idx = getStartCharsBufIdx(peekChars,numPeekChars, NULL, 0);
  if (idx >= 0 && (numInFrameBuf-idx >= minCharsNeeded) ) {
    return idx; // Enough data for a frame so return idx of startChars
  }
  return -1; // Indicates not enough data and/or no start chars found
}



