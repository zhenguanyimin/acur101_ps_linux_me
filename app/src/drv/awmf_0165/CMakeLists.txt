#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(app)
#add_subdirectory(cfg)
#add_subdirectory(drv)

aux_source_directory(. DIR_LIB_SRCS)
add_library(awmf_0165 ${DIR_LIB_SRCS})
set(AWMF_0165_LIBS awmf_0165 PARENT_SCOPE)

