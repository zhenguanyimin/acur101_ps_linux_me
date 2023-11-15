#
#

aux_source_directory(. DIR_LIB_SRCS)
add_library(version ${DIR_LIB_SRCS})
set(VERSION_LIBS version PARENT_SCOPE)
