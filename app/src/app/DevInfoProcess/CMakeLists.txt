#
#
#aux_source_directory(. DIR_SCRS)

aux_source_directory(. DIR_LIB_SRCS)
add_library(DevInfoProcess ${DIR_LIB_SRCS})
set (DEVINFOPROCESS_LIBS DevInfoProcess PARENT_SCOPE)


