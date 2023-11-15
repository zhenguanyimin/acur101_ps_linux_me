#
#

add_subdirectory(udp_client)

#aux_source_directory(./perf DIR_LIBS_SRCS)
#add_library(perf ${DIR_LIBS_SRCS})
set(PERF_LIBS ${UDP_CLIENT_LIBS} PARENT_SCOPE)

