#
#

add_subdirectory(sysport)
add_subdirectory(storage)
add_subdirectory(platfrom)
add_subdirectory(monitor)

aux_source_directory(. DIR_LIBS_SRCS)
add_library(service ${DIR_LIBS_SRCS})
set(SERVICE_LIBS service 
	${SYSPORT_LIBS}
	${STORAGE_LIBS}
	${PLATFROM_LIBS}
	${MONITOR_LIBS}
	PARENT_SCOPE)
