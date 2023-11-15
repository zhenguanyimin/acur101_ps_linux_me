#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(c2_network)
#add_subdirectory(DataPath)

aux_source_directory(. DIR_LIB_SRCS)
add_library(monitor_temp ${DIR_LIB_SRCS})

set(MONITOR_TEMP_LIBS monitor_temp PARENT_SCOPE)

