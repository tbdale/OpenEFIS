
#define MILES_PER_NM    1.15077945
#define KM_PER_MILE     1.609344

// nautical miles per meter
#define NM_PER_METER            0.000539956803
#define KNOTS_PER_METER_S       1.9438416
#define NEWTON_PER_LBF          4.4472216152 

#define DEGREES_PER_RADIAN      (180.0 / M_PI)

#define LOCAL_GRAVITY           9.80665 /* m/s^2 */

#define NELEMENTS(a)    (sizeof(a)/sizeof(a[0]))

#define TRUE    (1 == 1)
#define FALSE   (1 == 0)

#define DEBUG (getenv("DEBUG") != NULL)

// Update interval the period in uS: 100 ms
#define  UPDATE_INTERVAL     100000

