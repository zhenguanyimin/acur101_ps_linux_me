#
#

add_subdirectory(index)
add_subdirectory(func)
add_subdirectory(altp)
add_subdirectory(cli)
add_subdirectory(remote)

aux_source_directory(. DIR_LIBS_SRCS)
add_library(sfbus ${DIR_LIBS_SRCS})
set(SFBUS_LIBS sfbus 
	${SFBUS_INDEX_LIBS}
	${SFBUS_FUNC_LIBS}
	${SFBUS_ALTP_LIBS}
	${SFBUS_CLI_LIBS}
	${SFBUS_REMOTE_LIBS}
	PARENT_SCOPE)

