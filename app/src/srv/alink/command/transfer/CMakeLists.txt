#
#

aux_source_directory(. DIR_LIBS_SRCS)
add_library(transfer ${DIR_LIBS_SRCS})
set(TRANSFER_LIBS transfer PARENT_SCOPE)

