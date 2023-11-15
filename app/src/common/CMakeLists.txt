#
#
#aux_source_directory(. DIR_SCRS)

add_subdirectory(table_2d)

#aux_source_directory(. DIR_LIB_SRCS)
#add_library(main ${DIR_LIB_SRCS})
set(COMMON_LIBS 
    ${TABLE_2D_LIBS} 
    PARENT_SCOPE)
