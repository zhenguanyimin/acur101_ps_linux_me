#
#
#aux_source_directory(. DIR_SCRS)

aux_source_directory(. DIR_LIB_SRCS)
add_library(network_monitor ${DIR_LIB_SRCS})

set(NETWORK_MONITOR_LIBS network_monitor PARENT_SCOPE)

