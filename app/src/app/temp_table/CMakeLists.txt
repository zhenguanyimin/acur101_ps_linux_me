#
#
#aux_source_directory(. DIR_SCRS)

aux_source_directory(. DIR_LIB_SRCS)
add_library(temp_table ${DIR_LIB_SRCS})
set (TEMP_TABLE_LIBS temp_table PARENT_SCOPE)


