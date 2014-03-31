// xplane_interface.h: Definition for the interface to the X-Plane flight simulator
//                     for testing.
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

enum {
    XPT_FRAME_RATE,
    XPT_ELAPSE_TIME,
    XPT_SPEED_VSI,
    XPT_JOYSTICK = 8,
    XPT_ART_STAB,
    XPT_FLT_CONTROL,
    XPT_WINGSWEEP_THRUST_VEC,
    XPT_TRIM_FLAP,
    XPT_GEAR_BRAKES,
    XPT_ANG_MOMENTS,
    XPT_ANG_ACCEL,
    XPT_ANG_VEL,
    XPT_PRH,
    XPT_LAT_LONG_ALT,
    XPT_XYZ,
    XPT_THROTTLE_SETTING = 23,
    XPT_ENGINE_SETTING,
    XPT_PROP_SETTING,
    XPT_MIXTURE_SETTING,
    XPT_COWL_FLAP,
    XPT_ENG_THRUST = 31,
    XPT_AERO_FORCE = 53,
    XPT_NAV_FREQ = 83,
    XPT_NAV_OBS,
    XPT_NAV_DEFLECT,
    XPT_GPS = 88
};

const unsigned  SIZEOF_XPT_DATA = sizeof(int) + (8*sizeof(float));

#define  XP_FRAME_RATE 20
