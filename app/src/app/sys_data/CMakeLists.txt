#
#
#aux_source_directory(. DIR_SCRS)

aux_source_directory(. DIR_LIB_SRCS)
add_library(sys_data ${DIR_LIB_SRCS})
set (SYS_DATA_LIBS sys_data PARENT_SCOPE)


