#
#
#aux_source_directory(. DIR_SCRS)

add_subdirectory(src)

#aux_source_directory(. DIR_LIB_SRCS)
#add_library(measurementProcess ${DIR_LIB_SRCS})
set (MEASUREMENTPROCESS_LIBS
	${MEASUREMENTPROCESS_SRC_LIBS}
	PARENT_SCOPE)


