#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(c2_network)
#add_subdirectory(DataPath)

aux_source_directory(. DIR_LIB_SRCS)
add_library(beam_calculate ${DIR_LIB_SRCS})

set(BEAM_CALCULATE_LIBS beam_calculate PARENT_SCOPE)

