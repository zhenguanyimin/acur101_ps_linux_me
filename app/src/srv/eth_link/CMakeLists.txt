#
#

add_subdirectory(socket)

aux_source_directory(. DIR_LIBS_SRCS)
add_library(eth_link ${DIR_LIBS_SRCS})
set(ETH_LINK_LIBS eth_link
	${SOCKET_LIBS}
	PARENT_SCOPE)
