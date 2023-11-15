#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(alink)


aux_source_directory(. DIR_LIB_SRCS)
add_library(flash_nv ${DIR_LIB_SRCS})
set(FLASH_NV_LIBS flash_nv PARENT_SCOPE)


