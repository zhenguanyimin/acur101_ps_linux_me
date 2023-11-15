#
#

aux_source_directory(. DIR_LIB_SRCS)
add_library(pl_test ${DIR_LIB_SRCS})
set(PL_TEST_LIBS pl_test PARENT_SCOPE)
