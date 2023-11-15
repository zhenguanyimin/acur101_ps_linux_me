#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(c2_network)
#add_subdirectory(DataPath)

aux_source_directory(. DIR_LIB_SRCS)
add_library(monitor_power ${DIR_LIB_SRCS})

set(MONITOR_POWER_LIBS monitor_power PARENT_SCOPE)

