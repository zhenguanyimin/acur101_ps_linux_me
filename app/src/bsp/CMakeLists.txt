#
#
#aux_source_directory(. DIR_SCRS)

add_subdirectory(reg)

#aux_source_directory(. DIR_LIB_SRCS)
#add_library(main ${DIR_LIB_SRCS})

set(BSP_LIBS 
    ${REG_LIBS} 
    PARENT_SCOPE)


