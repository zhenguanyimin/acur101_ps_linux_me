#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(src)

aux_source_directory(. DIR_LIB_SRCS)
add_library(magCalibration4 ${DIR_LIB_SRCS})
set (MAGCALIBRATION4_LIBS magCalibration4 PARENT_SCOPE)


