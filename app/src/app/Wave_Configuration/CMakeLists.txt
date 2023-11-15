#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(c2_network)
#add_subdirectory(DataPath)

aux_source_directory(. DIR_LIB_SRCS)
add_library(Wave_Configuration ${DIR_LIB_SRCS})

set(WAVE_CONFIGURATION_LIBS Wave_Configuration PARENT_SCOPE)

