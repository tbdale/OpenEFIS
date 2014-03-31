SOURCES	+= efis.cpp \
	main.cpp \
	pfd_asi.cpp \
	pfd.cpp \
	pfd_ah.cpp \
	pfd_alt.cpp \
	hsi.cpp \
	pfd_vsi.cpp \
#        ahrs_cooked.cpp \
#        compass.cpp \
        init_instruments.cpp \
        shadinZ.cpp \
        ahrs.cpp \
        differentiate.cpp \
        ahrs_xbow.cpp \
        nav.cpp \
        fad_fdatasystems.cpp \
#        nav_xplane.cpp \
#        ahrs_xplane.cpp \
        update_instruments.cpp \
        airspeed.cpp \
        gps.cpp \
        xpdr_sl70r.cpp \
#        airspeed_xplane.cpp \
        gps_ff.cpp \
        altitude.cpp \
        autopilot.cpp \
#        gps_xplane.cpp \
#        autopilot_xplane.cpp \
        serial.cpp\
        eis/eis.cpp\
        eis/eis_tach.cpp\
        eis/eis_map.cpp\
        eis/eis_oilpressure.cpp\
        eis/eis_oiltemp.cpp\
        stamp_sensors.cpp


HEADERS	+= efis.h \
	pfd.h \
	hsi.h \
        ahrs_cooked.h \
        altitude_xplane.h \
        differentiate.h \
        gps.h \
        shadinZ.h \
        ahrs.h \
        autopilot.h \
        display_interface.h \
        gps_xplane.h \
        syntax_error.h \
        ahrs_xbow.h \
        autopilot_xplane.h \
        ahrs_xplane.h \
        comm_sl40.h \
        exceptions.h \
        nav.h \
        utilities.h \
        airspeed.h \
        compass.h \
        fad_fdatasystems.h \
        nav_xplane.h \
        xpdr_sl70r.h \
        airspeed_xplane.h \
        compass_xplane.h \
        fpm.h \
        altitude.h \
        constants.h \
        gps_ff.h \
        serial.h \
        eis/eis.h \
        stamp_sensors.h

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}

TEMPLATE	=app
CONFIG	+= qt opengl thread warn_on release
LANGUAGE	= C++
