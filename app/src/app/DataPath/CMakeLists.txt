#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(c2_network)
#add_subdirectory(DataPath)

aux_source_directory(. DIR_LIB_SRCS)
add_library(datapath ${DIR_LIB_SRCS})

set(DATAPATH_LIBS datapath PARENT_SCOPE)

