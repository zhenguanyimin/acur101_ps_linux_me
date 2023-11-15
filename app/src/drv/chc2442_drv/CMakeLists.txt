#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(app)
#add_subdirectory(cfg)
#add_subdirectory(drv)

aux_source_directory(. DIR_LIB_SRCS)
add_library(chc2442_drv ${DIR_LIB_SRCS})
set(CHC2442_DRV_LIBS chc2442_drv PARENT_SCOPE)

