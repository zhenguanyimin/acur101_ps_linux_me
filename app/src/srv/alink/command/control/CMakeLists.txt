#
#

aux_source_directory(. DIR_LIBS_SRCS)
add_library(control ${DIR_LIBS_SRCS})
set(CONTROL_LIBS control PARENT_SCOPE)

