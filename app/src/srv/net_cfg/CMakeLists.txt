#
#

aux_source_directory(. DIR_LIBS_SRCS)
add_library(net_cfg ${DIR_LIBS_SRCS})
set(NET_CFG_LIBS net_cfg 
	PARENT_SCOPE)

