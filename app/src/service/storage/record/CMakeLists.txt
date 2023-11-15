#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(c2_network)
#add_subdirectory(DataPath)

aux_source_directory(. DIR_LIB_SRCS)
add_library(storage_record ${DIR_LIB_SRCS})

set(STORAGE_RECORD_LIBS storage_record PARENT_SCOPE)

