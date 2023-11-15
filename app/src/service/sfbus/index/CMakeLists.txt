#
#add_subdirectory(func)

aux_source_directory(. DIR_LIB_SRCS)
add_library(sfbus_index ${DIR_LIB_SRCS})

set(SFBUS_INDEX_LIBS sfbus_index PARENT_SCOPE)

