// differentiate.h: Class definition for generic diffentiator (take the 1st derivative)
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

#ifndef DIFFERENTIATE_H
#define DIFFERENTIATE_H

#include <assert.h>
#include <stdlib.h>

#include "constants.h"

// differentiate class: An abstraction for storing historical samples and being
// able to compute the first derivative of those samples. If the first derivative is
// small, this class will look back in history far enough to get at least 2 significant
// digits, or until the history buffer is exhausted.
class differentiate
{
    public:
        /**
         * AddSample
         * DESCRIPTION:     Adds a sample to the history buffer
         * PRE-CONDITIONS:  None
         * POST-CONDITIONS: None
         * EXCEPTIONS THROWN:  None
         * EXCEPTIONS HANDLED: None
         */
        void AddSample
            (
             int        value
            )
            {
                assert (history_buffer != NULL);
                // Increment and wrap t0_index
                ++t0_index;
                if (t0_index >= history_size)
                    t0_index = 0;
                if (elements_filled == 0)
                    history_buffer [t0_index] = 0;
                else
                    history_buffer [t0_index] = value - last_value;
                if (elements_filled < history_size)
                    elements_filled++;
                last_value = value;
            }

        /**
         * HistoryDepth
         * DESCRIPTION:     Returns the depth of the history buffers
         * PRE-CONDITIONS:  None
         * POST-CONDITIONS: None
         * EXCEPTIONS THROWN:  None
         * EXCEPTIONS HANDLED: None
         */
        unsigned  // See Description
            HistoryDepth (void) const
            {
                return (history_size);
            }

        /**
         * Differentiate
         * DESCRIPTION:     Returns the 1st derivative.
         *                  able to compute the first derivative of those samples.
         *                  If the first derivative is
         *                  small, this class will look back in history
         *                  far enough to get at least 2 significant
         *                  digits, or until the history buffer is exhausted.
         * PRE-CONDITIONS:  None
         * POST-CONDITIONS: None
         * EXCEPTIONS THROWN:  NO_DATA
         * EXCEPTIONS HANDLED: None
         */
        float  // See Description
            Differentiate (void) const;

        differentiate
            (
             const unsigned     hist_size,
             const float       *filt,
             const unsigned     ntaps
            ) : history_size(hist_size), filter_taps(filt), filter_ntaps(ntaps)
            {
                elements_filled = 0;
                history_buffer = (int*) malloc (sizeof(int) * history_size);
                t0_index = 0;
            }
        ~differentiate (void)
        {
            if (history_buffer != NULL)
                free (history_buffer);
        }
    protected:
        int             last_value;             // The last value given
        const unsigned  history_size;
        unsigned        elements_filled;        // Starts at 0 and counts up to history_size
                                                // as samples are added. Permanently saturates
                                                // at history_size.
        int            *history_buffer;         // Stores the historical differences
        unsigned        t0_index;               // History index of most recent reading
                                                // Wraps around continuously as new
                                                // samples arrive
        const float    *filter_taps;            // Filter for computing 1st derivative
        const unsigned  filter_ntaps;           // Number of taps in the filter
};

#endif /* DIFFERENTIATE_H */
