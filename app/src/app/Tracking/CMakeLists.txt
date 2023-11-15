add_subdirectory(src)

aux_source_directory(. DIR_LIB_SRCS)
add_library(Tracking ${DIR_LIB_SRCS})

set(TRACKING_LIBS Tracking 
	${TRACK_SRC_LIBS}
	PARENT_SCOPE)

