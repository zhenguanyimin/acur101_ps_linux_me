#
#
#aux_source_directory(. DIR_SCRS)

aux_source_directory(. DIR_LIB_SRCS)
add_library(utilities ${DIR_LIB_SRCS})
set (UTILITIES_LIBS utilities PARENT_SCOPE)


