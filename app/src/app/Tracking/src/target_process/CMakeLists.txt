add_subdirectory(dispatch)
add_subdirectory(magCalibration4)
add_subdirectory(measurementProcess)
add_subdirectory(radarPoseEstimation)
add_subdirectory(targetThreatAssessment)
add_subdirectory(trackerExistingConfCoef)
add_subdirectory(tracking)

#aux_source_directory(. DIR_LIB_SRCS)
#add_library(target_process ${DIR_LIB_SRCS})

set(TARGET_PROCESS_LIBS
	${DISPATCH_LIBS}
	${MAGCALIBRATION4_LIBS}
	${MEASUREMENTPROCESS_LIBS}
	${RADARPOSEESTIMATION_LIBS}
	${TARGETTHREATASSESSMENT_LIBS}
	${TRACKEREXISTINGCONFCOEF_LIBS}
	${TRACKING_SRC_LIBS}
	PARENT_SCOPE)

