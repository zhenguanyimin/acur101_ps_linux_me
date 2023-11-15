#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(DataPath)

aux_source_directory(. DIR_LIB_SRCS)
add_library(reg ${DIR_LIB_SRCS})

set(REG_LIBS reg PARENT_SCOPE)

