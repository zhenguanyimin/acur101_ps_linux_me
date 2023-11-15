#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(lmx2492_hal)

aux_source_directory(. DIR_LIB_SRCS)
add_library(lmx2492_hal ${DIR_LIB_SRCS})
set(LMX2492_HAL_LIBS lmx2492_hal PARENT_SCOPE)


