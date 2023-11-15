#
#
#aux_source_directory(. DIR_SCRS)

add_subdirectory(src)

#aux_source_directory(. DIR_LIB_SRCS)
#add_library(targetThreatAssessment ${DIR_LIB_SRCS})
set (TARGETTHREATASSESSMENT_LIBS
	${TARGETTHREATASSESSMENT_SRC_LIBS}
	PARENT_SCOPE)


