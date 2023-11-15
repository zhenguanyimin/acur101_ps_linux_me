#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(alink)


aux_source_directory(. DIR_LIB_SRCS)
add_library(table_2d ${DIR_LIB_SRCS})
set(TABLE_2D_LIBS table_2d PARENT_SCOPE)


