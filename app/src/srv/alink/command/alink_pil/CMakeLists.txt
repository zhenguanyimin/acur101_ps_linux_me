#
#

aux_source_directory(. DIR_LIBS_SRCS)
add_library(alink_pil ${DIR_LIBS_SRCS})
set(ALINK_PIL_LIBS alink_pil PARENT_SCOPE)

