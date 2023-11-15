#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(c2_network)
#add_subdirectory(DataPath)

aux_source_directory(. DIR_LIB_SRCS)
add_library(platfrom_attitude ${DIR_LIB_SRCS})

set(PLATFROM_ATTITUDE_LIBS platfrom_attitude PARENT_SCOPE)

