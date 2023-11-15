#
#

add_subdirectory(attitude)
add_subdirectory(gps)

aux_source_directory(. DIR_LIBS_SRCS)
add_library(platfrom ${DIR_LIBS_SRCS})
set(PLATFROM_LIBS platfrom 
	${PLATFROM_ATTITUDE_LIBS}
	${PLATFROM_GPS_LIBS}
	PARENT_SCOPE)

