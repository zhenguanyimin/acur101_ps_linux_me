#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(c2_network)
#add_subdirectory(DataPath)

aux_source_directory(. DIR_LIB_SRCS)
add_library(platfrom_gps ${DIR_LIB_SRCS})

set(PLATFROM_GPS_LIBS platfrom_gps PARENT_SCOPE)

