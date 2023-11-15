#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(c2_network)
#add_subdirectory(DataPath)

aux_source_directory(. DIR_LIB_SRCS)
add_library(sfbus_altp ${DIR_LIB_SRCS})

set(SFBUS_ALTP_LIBS sfbus_altp PARENT_SCOPE)
