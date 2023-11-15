#
#

aux_source_directory(. DIR_LIBS_SRCS)
add_library(upgrade ${DIR_LIBS_SRCS})
set(UPGRADE_LIBS upgrade PARENT_SCOPE)

