#
#

aux_source_directory(. DIR_LIBS_SRCS)
add_library(alink_record ${DIR_LIBS_SRCS})
set(ALINK_RECORD_LIBS alink_record PARENT_SCOPE)

