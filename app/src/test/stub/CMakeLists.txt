#
#

aux_source_directory(. DIR_LIBS_SRCS)
add_library(stub ${DIR_LIBS_SRCS})
set(STUB_LIBS stub PARENT_SCOPE)


