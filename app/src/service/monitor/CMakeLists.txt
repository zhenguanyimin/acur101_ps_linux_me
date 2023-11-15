#
#

add_subdirectory(temp)
add_subdirectory(power)
add_subdirectory(network)

aux_source_directory(. DIR_LIBS_SRCS)
add_library(monitor ${DIR_LIBS_SRCS})
set(MONITOR_LIBS monitor 
	${MONITOR_TEMP_LIBS}
	${MONITOR_POWER_LIBS}
	${NETWORK_MONITOR_LIBS}
	PARENT_SCOPE)

