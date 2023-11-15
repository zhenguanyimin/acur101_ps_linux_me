#
#

add_subdirectory(control)
add_subdirectory(system)
add_subdirectory(track)
add_subdirectory(transfer)
add_subdirectory(upgrade)
add_subdirectory(record)
add_subdirectory(alink_pil)

#aux_source_directory(. DIR_LIBS_SRCS)
#add_library(command ${DIR_LIBS_SRCS})
set(COMMAND_LIBS 
	${CONTROL_LIBS}
	${SYSTEM_LIBS}
	${TRACK_LIBS}
	${TRANSFER_LIBS}
	${UPGRADE_LIBS}
	${ALINK_RECORD_LIBS}
	${ALINK_PIL_LIBS}
	PARENT_SCOPE)
