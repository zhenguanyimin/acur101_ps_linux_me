#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(app)
#add_subdirectory(cfg)
#add_subdirectory(chc2442_drv)

aux_source_directory(. DIR_LIB_SRCS)
add_library(lsm303ah ${DIR_LIB_SRCS})

set(LSM303AH_LIBS lsm303ah PARENT_SCOPE)

