#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(src)

aux_source_directory(. DIR_LIB_SRCS)
add_library(radarposeestimation_src ${DIR_LIB_SRCS})
set (RADARPOSEESTIMATION_SRC_LIBS radarposeestimation_src PARENT_SCOPE)


