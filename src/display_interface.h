// display_interface.h: Class definition of display interface. Communicates with
// the GUI through socket I/O. Communicates commands taken from the GUI and sends
// back results.
//
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
// Interface Protocol:
//   If interface receives the character 'P', it will return
//   PFD display information in the following format:
//      pitch_angle(float, in radians)
//      roll_angle(float, in radians)
//      yaw_angle(float, in radians)
//      airspeed(unsigned, in knots)
//      altitude(int, in feet MSL)
//      VSI(unsigned, in fpm)
//
//
//      For any of the above data, the string "UNKNOWN" may be substituted.
