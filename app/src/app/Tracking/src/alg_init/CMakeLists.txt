#
#
#aux_source_directory(. DIR_SCRS)

aux_source_directory(. DIR_LIB_SRCS)
add_library(track_alg_init ${DIR_LIB_SRCS})
set (TRACK_ALG_INIT_LIBS track_alg_init PARENT_SCOPE)


