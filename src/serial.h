// serial.h: Class definition for the serial port class
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

#ifndef SERIAL_H
#define SERIAL_H

#define FRAME_BUF_SIZE 20000

class serial {
    public:
        int   numRead;
        int   numWrote;
        //unsigned char  *readCharBuf;
        unsigned char  readCharBuf[2048];

    protected:
        char  portName[80];
        int   portOpenFlags;
        int   asyncFlag;
        int   fd;
        int   readTimeoutMillis;
        unsigned char *startFrameChars;
        unsigned char *stopFrameChars;
	int startFrameCharCnt;
	int stopFrameCharCnt;
        int minFrameLen;
        int maxFrameLen;
	int fixedFrameLen;
	int numInFrameBuf;
	unsigned char frameDataBuf[FRAME_BUF_SIZE]; // TODO: sized correctly?

    public:
        /**
         * serial class constructor
         * DESCRIPTION:     Constructor to open and condition serial port
         * PRE-CONDITIONS:  A valid serial port and async flag is passed in
         * POST-CONDITIONS: Port is opened for IO at conditioned state
         * EXCEPTIONS THROWN:  TODO:???
         * EXCEPTIONS HANDLED: None
         */
        serial(const char* portPtr, int syncFlag);
        /**
         * serial class constructor
         * DESCRIPTION:     Constructor to open and condition serial port
         * PRE-CONDITIONS:  A valid serial port is passed in
         * POST-CONDITIONS: Port is opened for IO at conditioned state
         * EXCEPTIONS THROWN:  TODO:???
         * EXCEPTIONS HANDLED: None
         */
        serial(const char* portPtr);
        ~serial(); 

        /**
         * readPort
         * DESCRIPTION:     Blocks for "readTimeoutMillis" read of "numChars"
         *                  from the port.
         * PRE-CONDITIONS:  Valid port
         * POST-CONDITIONS: Valid port
         * EXCEPTIONS THROWN:  TODO: ????
         * EXCEPTIONS HANDLED: None
         */
        int readPort(int numChars);

        /**
         * writePort
         * DESCRIPTION:     Writes characters to serial port.
         * PRE-CONDITIONS:  Valid port
         * POST-CONDITIONS: Valid port
         * EXCEPTIONS THROWN:  TODO: ????
         * EXCEPTIONS HANDLED: None
         */
        int writePort(char* charBuf, int numChars);

        /**
         * setBaud
         * DESCRIPTION:     Set baud rate on open port
         * PRE-CONDITIONS:  Valid hardware port
         * POST-CONDITIONS: Port set to new baud rate
         * EXCEPTIONS THROWN:  TODO:???
         * EXCEPTIONS HANDLED: 
         */
        void setBaud(int baud);

        /**
         * setReadTimeout
         * DESCRIPTION:     Set read timeout in tenths of secs
         * PRE-CONDITIONS:  Valid hardware port
         * POST-CONDITIONS: Timeout set to new value
         * EXCEPTIONS THROWN:  TODO:???
         * EXCEPTIONS HANDLED: 
         */
        void setReadTimeout(int tenthsOfSecs);

        /**
         * setHwFlowControl
         * DESCRIPTION:     Enable/disable hardware flow ctl on port
         * PRE-CONDITIONS:  Valid hardware port
         * POST-CONDITIONS: Port set to new h/w flow state
         * EXCEPTIONS THROWN:  TODO:???
         * EXCEPTIONS HANDLED: 
         */

        /**
         * inititalizePort
         * DESCRIPTION:     opens and conditions port
         * PRE-CONDITIONS:  Open or closed hardware port
         * POST-CONDITIONS: Port open and ready for i/o
         * EXCEPTIONS THROWN:  TODO:???
         * EXCEPTIONS HANDLED: 
         */
        void initializePort();

        void setHwFlowControl(int tenthsOfSecs);
	void setMinMaxFrameLen(int shortest, int longest);
	void setFixedFrameLen(int frameLen);
	void setFrameStart(unsigned char* startChars, int numChars);
	void setFrameEnd(unsigned char* endChars, int numChars);
	int peekBufForStartCharsPlusFrameLength(unsigned char *peekChars, 
					   int numPeekChars, 
					   int minCharsNeeded);
        bool getFrame(unsigned char* framePtr);
	int getStartCharsBufIdx(unsigned char *startChars, 
				int startCharCnt, 
				unsigned char *stopChars, 
				int stopCharCnt);
	void readAvailableData(void);

	static bool checksumGood(unsigned char *framePtr, 
			  int startCheckIdx,
			  int endCheckIdx,
			  int checksumIdx);

	static void byteswap(void *, int);

        /**
         * getFrameFromBufAndShift
         * DESCRIPTION:    Extracts packets from serial buffer and shifts buffer
         * PRE-CONDITIONS:  None
         * POST-CONDITIONS: None
         * EXCEPTIONS THROWN:  None
         * EXCEPTIONS HANDLED: None
         */
	void getFrameFromBufAndShift(unsigned char *pktBuf, 
                                      int bufIdx, 
                                      int packetLen);




    protected:
        /**
         * openPort
         * DESCRIPTION:     Opens and conditions serial port
         * PRE-CONDITIONS:  Valid hardware port
         * POST-CONDITIONS: Port ready for IO
         * EXCEPTIONS THROWN:  TODO:???
         * EXCEPTIONS HANDLED: 
         */
        void openPort();
        /**
         * closePort
         * DESCRIPTION:     Close serial port
         * PRE-CONDITIONS:  Open or closed hardware port
         * POST-CONDITIONS: Port closed
         * EXCEPTIONS THROWN:  TODO:???
         * EXCEPTIONS HANDLED: 
         */
        void closePort();


};

// This will be seen by the compiler only once 
#endif /* SERIAL_H */
