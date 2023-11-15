#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(src)

aux_source_directory(. DIR_LIB_SRCS)
add_library(tracking_src_src ${DIR_LIB_SRCS})
set (TRACKING_SRC_SRC_LIBS  tracking_src_src PARENT_SCOPE)


