#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(lmx2492_hal)

aux_source_directory(. DIR_LIB_SRCS)
add_library(gyro ${DIR_LIB_SRCS})
set(GYRO_LIBS gyro PARENT_SCOPE)


