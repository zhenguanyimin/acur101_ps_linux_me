add_subdirectory(alg_init)
add_subdirectory(target_process)
add_subdirectory(utilities)

#aux_source_directory(. DIR_LIB_SRCS)
#add_library(track_src ${DIR_LIB_SRCS})

set(TRACK_SRC_LIBS
	${TRACK_ALG_INIT_LIBS}
	${TARGET_PROCESS_LIBS}
	${UTILITIES_LIBS}
	PARENT_SCOPE)

