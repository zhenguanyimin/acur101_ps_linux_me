#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(app)
#add_subdirectory(cfg)
#add_subdirectory(drv)

aux_source_directory(. DIR_LIB_SRCS)
add_library(lmx2492_drv ${DIR_LIB_SRCS})
set(LMX2492_DRV_LIBS lmx2492_drv PARENT_SCOPE)

