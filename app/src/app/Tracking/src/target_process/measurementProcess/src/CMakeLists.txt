#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(src)

aux_source_directory(. DIR_LIB_SRCS)
add_library(measurementprocess_src ${DIR_LIB_SRCS})
set (MEASUREMENTPROCESS_SRC_LIBS measurementprocess_src PARENT_SCOPE)


