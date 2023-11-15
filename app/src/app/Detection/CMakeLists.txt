
aux_source_directory(. DIR_LIB_SRCS)
add_library(Detection ${DIR_LIB_SRCS})

set(DETECTION_LIBS Detection PARENT_SCOPE)

