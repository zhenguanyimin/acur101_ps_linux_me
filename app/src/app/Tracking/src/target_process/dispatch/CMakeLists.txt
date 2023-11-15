#
#
#aux_source_directory(. DIR_SCRS)

add_subdirectory(src)

#aux_source_directory(. DIR_LIB_SRCS)
#add_library(dispatch ${DIR_LIB_SRCS})
set (DISPATCH_LIBS
	${DISPATCH_SRC_LIBS}
	PARENT_SCOPE)


