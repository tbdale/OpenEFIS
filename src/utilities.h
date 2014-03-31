
#include <sys/time.h>

/**
 * time_in_us
 * DESCRIPTION:     Compute time since 1970 in microseconds from the timeval
 *                  structure.
 * PRE-CONDITIONS:  
 * POST-CONDITIONS:
 * EXCEPTIONS THROWN:  
 * EXCEPTIONS HANDLED: 
 */
inline unsigned time_in_us (void)
{
    struct timeval      tv;
    gettimeofday (&tv, NULL);
    return (tv.tv_sec * 1000000 + tv.tv_usec);
}
