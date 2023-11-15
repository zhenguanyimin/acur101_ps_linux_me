#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(c2_network)
#add_subdirectory(DataPath)

aux_source_directory(. DIR_LIB_SRCS)
add_library(pil ${DIR_LIB_SRCS})

set(PIL_LIBS pil PARENT_SCOPE)

