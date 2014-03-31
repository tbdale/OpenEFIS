// test_differentiate.cpp: Standalone test suite for the differentiate class
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

#include "differentiate.h"

float   test_filt1 [] =
{
    0.6, 0.3, 0.07, 0.03
};

float   test_filt2 [20];

int main (int argc, char *argv[])
{
    unsigned            i;
    float               tap;
    differentiate      *test1, *test2;

    tap = .5;
    for (i = 0; i < NELEMENTS(test_filt2)-1; i++)
    {
        test_filt2 [i] = tap;
        tap /= 2;
    }
    test_filt2 [NELEMENTS(test_filt2)-1] = test_filt2 [NELEMENTS(test_filt2)-2]; 

    test1 = new differentiate (NELEMENTS(test_filt1), test_filt1, NELEMENTS(test_filt1));
    test2 = new differentiate (100, test_filt2, NELEMENTS(test_filt2));

    printf ("\n\ntestpat1: sine wave\n"
                "------------------\n");
    for (i = 0; i < 100; i++)
    {
        test1->AddSample ((int)(1000 * sin (i * M_PI / 50)));
        test2->AddSample ((int)(1000 * sin (i * M_PI / 50)));
        printf ("t1 = %10f, t2 = %10f\n", test1->Differentiate(), test2->Differentiate());
    }
    printf ("\n\ntestpat2: Parabola\n"
                "------------------\n");
    for (i = 0; i < 100; i++)
    {
        test1->AddSample ((int)(i * i));
        test2->AddSample ((int)(i * i));
        printf ("t1 = %10f, t2 = %10f\n", test1->Differentiate(), test2->Differentiate());
    }
    printf ("\n\ntestpat3: micro slope line\n"
                "------------------\n");
    for (i = 0; i < 100; i++)
    {
        test1->AddSample ((int)(i * .07));
        test2->AddSample ((int)(i * .07));
        printf ("t1 = %10f, t2 = %10f\n", test1->Differentiate(), test2->Differentiate());
    }
    printf ("\n\ntestpat4: small slope line\n"
                "------------------\n");
    for (i = 0; i < 100; i++)
    {
        test1->AddSample ((int)(i));
        test2->AddSample ((int)(i));
        printf ("t1 = %10f, t2 = %10f\n", test1->Differentiate(), test2->Differentiate());
    }

    delete test1;
    delete test2;
    return 0;
}
