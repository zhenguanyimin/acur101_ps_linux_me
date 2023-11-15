#
#
aux_source_directory(. DIR_LIBS_SRCS)
add_library(cli ${DIR_LIBS_SRCS})
set(CLI_LIBS cli PARENT_SCOPE)

