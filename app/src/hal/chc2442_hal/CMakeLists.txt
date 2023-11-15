#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(lmx2492_hal)

aux_source_directory(. DIR_LIB_SRCS)
add_library(chc2442_hal ${DIR_LIB_SRCS})
set(CHC2442_HAL_LIBS chc2442_hal PARENT_SCOPE)


