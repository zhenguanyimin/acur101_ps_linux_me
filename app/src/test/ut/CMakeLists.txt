#
#

add_subdirectory(perf)

aux_source_directory(. DIR_LIBS_SRCS)
add_library(ut ${DIR_LIBS_SRCS})
set(UT_LIBS ut ${PERF_LIBS} PARENT_SCOPE)

