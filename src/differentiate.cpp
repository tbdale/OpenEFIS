// differentiate.cpp: Class definition for generic diffentiator (take the 1st derivative)
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

#include <stdio.h>
#include <stdlib.h>
#include "differentiate.h"


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
    differentiate::Differentiate (void) const
{
    double      accumulator;
    unsigned    index, count;
    bool        nonzero;

    count = 0;
    index = t0_index;
    accumulator = 0;
    nonzero = FALSE;
    while (count < filter_ntaps)
    {
        int     h = history_buffer [index];
        if (count >= elements_filled)
            break;
        if ((h < -1) || (h > 1))
            nonzero = TRUE;
        accumulator += (filter_taps [count++] * h);
        if (index == 0)
            index = history_size - 1;
        else
            --index;
    }
    if ((nonzero == FALSE) && (elements_filled > filter_ntaps))
    {
        bool    positive, negative;
        // We have to search further back in history to get a small
        // reading for the first derivative
        count = 0;
        index = t0_index;
        accumulator = 0;
        positive = negative = FALSE;
        while (count < elements_filled)
        {
            int     h = history_buffer [index];
            if ((h < -1) || (h > 1))
                break;
            if (h == 1)
            {
                if (negative)
                    break;
                positive = TRUE;
            }
            if (h == -1)
            {
                if (positive)
                    break;
                negative = TRUE;
            }
            accumulator += h;
            if (index == 0)
                index = history_size - 1;
            else
                --index;
            count++;
        }
        accumulator /= count;
    }
    return ((float)accumulator);
}
