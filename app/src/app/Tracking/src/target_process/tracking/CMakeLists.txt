#
#
#aux_source_directory(. DIR_SCRS)

add_subdirectory(src)

#aux_source_directory(. DIR_LIB_SRCS)
#add_library(tracking_src ${DIR_LIB_SRCS})
set (TRACKING_SRC_LIBS
	${TRACKING_SRC_SRC_LIBS}
	PARENT_SCOPE)


