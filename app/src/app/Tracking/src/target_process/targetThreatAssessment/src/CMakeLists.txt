#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(src)

aux_source_directory(. DIR_LIB_SRCS)
add_library(targetthreatassessment_src ${DIR_LIB_SRCS})
set (TARGETTHREATASSESSMENT_SRC_LIBS  targetthreatassessment_src PARENT_SCOPE)


