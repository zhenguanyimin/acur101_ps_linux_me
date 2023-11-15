#
#
#aux_source_directory(. DIR_SCRS)

aux_source_directory(. DIR_LIB_SRCS)
add_library(c2_order ${DIR_LIB_SRCS})
set (C2_ORDER_LIBS c2_order PARENT_SCOPE)


