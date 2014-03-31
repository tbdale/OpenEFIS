// shadinZ.h: Class definition for the class that parses Shadin Fule/Air-data Z format
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

#define MAX_SHADIN_MSG_SIZE 256

class shadinZ {
    public:
      char rawMsg[MAX_SHADIN_MSG_SIZE];

      int iasKts;
      int tasKts;
      double mach;
      double pAlt;
      double dAlt;
      double oatDegC;
      double tatDegC;
      int windDir;
      int windSpeedKts;
      double rateTurnDegSec;
      int vSpeedFpm;
      int trueHeading;
      double rightFuelFlowGalHr;
      double rightFuelUsedGal;
      double leftFuelFlowGalHr;
      double leftFuelUsedGal;
      int err;
      int checksum1;
      int groundSpeedKts;
      int magTrackDeg;
      int distToDestNm;
      int magVarDeg;
      double latDeg;
      double lonDeg;
      int checksum2;


    protected:

    // Funcs
    public:
        shadinZ(const char unstrippedMsg[]);
	void parseShadinZFormat();
        void printValues(void);
    protected:   
	bool shadinZ::checksumOk(char *pktPtr, int dataIdxStart, int dataIdxStop, int checksum);

};
