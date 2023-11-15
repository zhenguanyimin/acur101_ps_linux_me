#
#

#add_subdirectory(udp_client)

aux_source_directory(. DIR_LIBS_SRCS)
add_library(udp_client ${DIR_LIBS_SRCS})
set(UDP_CLIENT_LIBS udp_client PARENT_SCOPE)

