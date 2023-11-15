#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(app)
#add_subdirectory(cfg)

aux_source_directory(. DIR_LIB_SRCS)
add_library(mygps ${DIR_LIB_SRCS})

set(MYGPS_LIBS mygps PARENT_SCOPE)

