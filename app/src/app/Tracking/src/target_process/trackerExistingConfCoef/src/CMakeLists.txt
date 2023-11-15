#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(src)

aux_source_directory(. DIR_LIB_SRCS)
add_library(trackerexistingconfcoef_src ${DIR_LIB_SRCS})
set (TRACKEREXISTINGCONFCOEF_SRC_LIBS  trackerexistingconfcoef_src PARENT_SCOPE)


