#
#

add_subdirectory(log)
add_subdirectory(file)
add_subdirectory(record)

aux_source_directory(. DIR_LIBS_SRCS)
add_library(storage ${DIR_LIBS_SRCS})
set(STORAGE_LIBS storage 
	${STORAGE_LOG_LIBS}
	${STORAGE_FILE_LIBS}
	${STORAGE_RECORD_LIBS}
	PARENT_SCOPE)
