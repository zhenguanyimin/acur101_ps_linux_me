#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(lmx2492_hal)

aux_source_directory(. DIR_LIB_SRCS)
add_library(led ${DIR_LIB_SRCS})
set(LED_LIBS led PARENT_SCOPE)


