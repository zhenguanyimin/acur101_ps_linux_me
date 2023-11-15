#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(src)

aux_source_directory(. DIR_LIB_SRCS)
add_library(dispatch_src ${DIR_LIB_SRCS})
set (DISPATCH_SRC_LIBS dispatch_src PARENT_SCOPE)


