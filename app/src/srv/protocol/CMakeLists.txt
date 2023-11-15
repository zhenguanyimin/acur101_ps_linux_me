#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(alink)


aux_source_directory(. DIR_LIB_SRCS)
add_library(protocol ${DIR_LIB_SRCS})
set(PROTOCOL_LIBS protocol PARENT_SCOPE)


