// fpm.h: Class definition for the flight plan manager
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

#include "syntax_error.h"

class fpm
{
    public:
        unsigned                heading;                        // Current desired heading
        unsigned                altitude;                       // Current desired altitude
        unsigned                bearing;                        // Bearing to next waypoint
        unsigned                tt_waypoint;                    // Time to next waypoint
        unsigned                tt_airport;                     // Time to next airport
        unsigned                tt_dest;                        // Time to final destination
        unsigned                flight_time;                    // Total flight time so far


        /**
         * TurnOff
         * DESCRIPTION:     Turn off the autopilot, and just calculate passive
         *                   display components like flight time.
         * PRE-CONDITIONS:  None
         * POST-CONDITIONS: The autopilot is no longer active
         * EXCEPTIONS THROWN:  None
         * EXCEPTIONS HANDLED: None
         */
        void TurnOff (void);


        /**
         * ExecuteFlightPlanFrom
         * DESCRIPTION:     Find the waypoint passed to this function and start the autopilot
         *                  moving toward it.
         * PRE-CONDITIONS:  There is a flight plan active and it contains the waypoint
         * POST-CONDITIONS: Autopilot is engaged and all FPM display components are displayed
         * EXCEPTIONS THROWN:  NO_FLIGHT_PLAN, NO_SUCH_WAYPOINT
         * EXCEPTIONS HANDLED: None
         */
        void ExecuteFlightPlanFrom
        (
         string         waypoint        // The waypoint to go to next.
        );


        /**
         * SuggestNextWaypoint
         * DESCRIPTION:     From the current GPS position, find a list of waypoints that
         *                  can be possibly picked up from to start executing the flight plan
         * PRE-CONDITIONS:  There is an active flight plan and GPS data is available.
         * POST-CONDITIONS: const
         * EXCEPTIONS THROWN:  NO_FLIGHT_PLAN, NO_GPS
         * EXCEPTIONS HANDLED: None
         */
        list_of_waypoint    // A list of possible waypoints to start executing from.
                            // Blank of there is no flight plan.
                            // If the aircraft is moving in a place and direction consistent
                            // with the flight plan, only one waypoint is returned.
        SuggestNextWaypoint (void) const;


        /**
         * TrackVOR
         * DESCRIPTION:     Start autopilot on a VOR track.
         * PRE-CONDITIONS:  VOR is on, connected and tuned, or alternately GPS info
         *                  is available.
         * POST-CONDITIONS: Autopilot is on and turning to an intercept track
         * EXCEPTIONS THROWN:  NO_VOR, VOR_NOT_TUNED
         * EXCEPTIONS HANDLED: None
         */
        void TrackVOR
        (
         string         vor_id, // VOR identifier. If null, just blindly follow VOR signals
         bool           to,     // True if it should go to the VOR, False if it should
                                // move away from the VOR
         unsigned       radial  // Radial to track on
        );

        /**
         * SetManual
         * DESCRIPTION:     Set the altitude and heading the autopilot
         *                  should go to and/or maintane
         * PRE-CONDITIONS:  None
         * POST-CONDITIONS: Autopilot in manual mode
         * EXCEPTIONS THROWN:  None
         * EXCEPTIONS HANDLED: None
         */
        void SetManual
        (
         unsigned       altitude,       // Altitude in feet MSL
         unsigned       heading         // Heading in degrees
        );

        /**
         * ExecuteApproachFrom
         * DESCRIPTION:     Execute an instrument approach to an airport
         * PRE-CONDITIONS:  None
         * POST-CONDITIONS: Autopilot is moving to the given waypoint of the approach
         * EXCEPTIONS THROWN:  NO_APT_DATA, NO_SUCH_APPROACH, NEEDS_PROC_TURN, BAD_ALTITUDE,
         *                     NO_GPS, NO_ALTITUDE
         *                     (BAD_ALTITUDE means the required altitude change for
         *                     such an approach is too much)
         * EXCEPTIONS HANDLED: None
         */
        void ExecuteApproachFrom
        (
         string         airport,        // Airport identifier or name
         string         approach,       // Approach name
         string         waypoint        // Waypoint name to start from
        );

        /**
         * SuggestNextApproachWaypoint
         * DESCRIPTION:     Find a list of approach waypoints that are close to
         *                  the current position. If the aircraft is flying a course
         *                  and position consistent with the approach, only one waypoint
         *                  is returned.
         * PRE-CONDITIONS:  None
         * POST-CONDITIONS: const
         * EXCEPTIONS THROWN:  NO_APT_DATA, NO_SUCH_APPROACH, NEEDS_PROC_TURN, BAD_ALTITUDE,
         *                     NO_GPS, NO_ALTITUDE
         *                     (BAD_ALTITUDE means the required altitude change for
         *                     such an approach is too much)
         * EXCEPTIONS HANDLED: None
         */
        list_of_waypoint    // A list of possible waypoints to start from
        SuggestNextApproachWaypoint
        (
         string         airport,        // Airport identifier or name
         string         approach        // Approach name
        );

        /**
         * LoadFlightPlan
         * DESCRIPTION:     Load the flight plan into the FPM
         * PRE-CONDITIONS:  FPM turned off.
         * POST-CONDITIONS: Flight plan data loaded
         * EXCEPTIONS THROWN:  FPM_ACTIVE, NO_SUCH_FILE
         * EXCEPTIONS HANDLED: None
         */
        SyntaxError  *          // Syntax error descriptor
                                // NULL if the file was read successfully.
                                // Caller must free pointer
        LoadFlightPlan
        (
         string         path    // Path to flight plan file.
        );

        /**
         * LoadIFRClearance
         * DESCRIPTION:     Load an IFR clearance.
         * PRE-CONDITIONS:  FPM not executing an existing clearance.
         * POST-CONDITIONS: clearance loaded
         * EXCEPTIONS THROWN:  FPM_ACTIVE, NO_SUCH_FILE
         * EXCEPTIONS HANDLED: None
         */
        SyntaxError  *          // Syntax error descriptor
                                // NULL if the file was read successfully.
                                // Caller must free pointer
        LoadIFRClearance
        (
         string         path    // Path to flight plan file.
        );




    protected:
        fpm_mode                mode;                           // Mode of operation
                                                                // One of: Off, plan, head_alt, VOR, ILS
        list_of_waypoint        clearance_route;
        unsigned                clearance_init_altitude;      // Initial altitude from clearance
        unsigned                clearance_expect_altitude;    // Expect altitude from clearance
        unsigned                clearance_expect_time;        // Minutes to expect altitude
        list_of_waypoint        planned_route;

        list_pointer            next_waypoint;
        list_pointer            next_airport;

        unsigned                vor_radial;                     // VOR Radial to intercept
        unsigned                vor_heading;                    // Which direction to go on
                                                                // that radial
        int                     vor_intercept;                  // Desired intercept angle for VOR
        int                     cdi;                            // Last CDI reading
        int                     gsi;                            // Last glide slope reading
        double                  dcdi;                           // CDI movement
        double                  dgsi;                           // GSI movement

        bool                    altitude_override;              // Override in plan mode.

        double                  altimeter;                      // Altimeter setting in "Hg
        unsigned                climb_airspeed[50];             // Climb airspeed for each altitude
                                                                // in thousands of feet.
        unsigned                descent_airspeed[50];           // Descent airspeed for each altitude
        unsigned                approach_descent_airspeed;      // Descent airspeed to use
                                                                // on a non-precision approach
        string                  selected_alternate;             // Alternate airport selected
        string                  selected_approach;              // Name of selected approach
                                                                // (NULL if visual)
}


class airport
{
    public:
        string                  name;                           // City or vernacular name
        string                  identifier;                     // FAA identifier
        list_of_aptid           aptattr;                        // Airport attributes

    protected:

}
