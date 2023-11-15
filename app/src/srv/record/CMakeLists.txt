#
#
aux_source_directory(. DIR_LIBS_SRCS)
add_library(record ${DIR_LIBS_SRCS})
set(RECORD_LIBS record PARENT_SCOPE)

