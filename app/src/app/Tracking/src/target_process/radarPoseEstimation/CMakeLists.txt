#
#
#aux_source_directory(. DIR_SCRS)

add_subdirectory(src)

#aux_source_directory(. DIR_LIB_SRCS)
#add_library(radarPoseEstimation ${DIR_LIB_SRCS})
set (RADARPOSEESTIMATION_LIBS
	${RADARPOSEESTIMATION_SRC_LIBS}
	PARENT_SCOPE)


